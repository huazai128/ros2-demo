#!/usr/bin/env python3
"""
AR0234 橙色小球连续跟踪

策略：
1) 全局橙色圆盘检测做首次锁定
2) 锁定后用 CSRT + 速度预测做连续跟踪（地面慢滚不易断）
3) 短暂丢失时在预测位置附近 ROI 内宽松重捕，并 coast 保持轨迹
"""

from __future__ import annotations

import argparse
import sys
import time
from collections import deque
from pathlib import Path

import cv2
import numpy as np

DEFAULT_MODEL = Path("runs/detect/runs/detect/ball_train/weights/best.pt")

# True ping-pong orange is high-S. Purple packaging also looks "orange-ish" under
# dim AR0234 light if S is too low — keep sat floors high for acquisition.
ORANGE_LO = np.array([8, 120, 40], dtype=np.uint8)
ORANGE_HI = np.array([24, 255, 255], dtype=np.uint8)
ORANGE_LO_LOOSE = np.array([6, 90, 30], dtype=np.uint8)
ORANGE_HI_LOOSE = np.array([26, 255, 255], dtype=np.uint8)
MIN_MEAN_S = 140.0
MIN_MEAN_S_LOOSE = 110.0


def _make_csrt():
    if hasattr(cv2, "TrackerCSRT_create"):
        return cv2.TrackerCSRT_create()
    if hasattr(cv2, "legacy") and hasattr(cv2.legacy, "TrackerCSRT_create"):
        return cv2.legacy.TrackerCSRT_create()
    return None


class BallTracker:
    def __init__(
        self,
        min_radius: float = 12.0,
        max_radius: float = 90.0,
        coast_frames: int = 12,
        reacquire_expand: float = 5.0,
        border_margin: int = 50,
    ):
        self.min_radius = min_radius
        self.max_radius = max_radius
        self.coast_frames = coast_frames
        self.reacquire_expand = reacquire_expand
        self.border_margin = border_margin

        self.track_id = 1
        self.history: deque = deque(maxlen=90)
        self.last_cxcy: tuple[float, float] | None = None
        self.last_radius: float = 30.0
        self.vx = 0.0
        self.vy = 0.0
        self.missed = 0
        self.lock_frames = 0
        self.stale_frames = 0
        self.prev_time = time.time()
        self.fps = 0.0
        self.state = "searching"

    def process_frame(self, frame):
        """Detection-first tracking: never stick to an empty old spot."""
        now = time.time()
        dt = max(1e-3, now - self.prev_time)
        self.fps = 1.0 / dt
        self.prev_time = now
        h, w = frame.shape[:2]
        annotated = frame.copy()

        # Always look for the real orange disk this frame.
        glob = self._detect_global(frame)
        local = None
        if self.last_cxcy is not None:
            speed = float(np.hypot(self.vx, self.vy))
            # Wider ROI when moving so a rolling ball isn't left behind.
            expand = self.reacquire_expand * (1.0 + min(speed, 800.0) / 400.0)
            pred = self._predict(dt)
            local = self._detect_in_roi(
                frame, pred, radius_hint=self.last_radius, expand=expand
            )

        # If old lock point no longer looks like a ball, drop it immediately.
        if self.last_cxcy is not None:
            q = self._orange_quality(
                frame, self.last_cxcy[0], self.last_cxcy[1], self.last_radius
            )
            if q["mean_s"] < 90 or q["fill"] < 0.12:
                self.stale_frames += 1
            else:
                self.stale_frames = 0
            if self.stale_frames >= 2:
                # Ball left; don't keep drawing the ghost marker.
                self._reset_track(keep_id=True)

        chosen = self._choose_detection(glob, local, dt)
        detection = None

        if chosen is not None:
            src = chosen.pop("_src", "det")
            detection = self._accept(
                chosen["cx"], chosen["cy"], chosen["radius"], now, source=src
            )
            self.missed = 0
            self.lock_frames += 1
            self.stale_frames = 0
            self.state = "BALL"
            self._draw_det(annotated, detection)
        else:
            self.missed += 1
            speed = float(np.hypot(self.vx, self.vy))
            can_coast = (
                self.last_cxcy is not None
                and self.missed <= self.coast_frames
                and speed > 40.0  # only coast while actually moving
            )
            if can_coast:
                pred_cx, pred_cy = self._predict(dt)
                # Don't coast into a spot that still "looks orange" from tablecloth
                # while a better global ball exists — already handled above.
                self.last_cxcy = (pred_cx, pred_cy)
                self.history.append((pred_cx, pred_cy, now))
                self.state = f"coast ({self.missed})"
                detection = {
                    "id": self.track_id,
                    "cx": pred_cx,
                    "cy": pred_cy,
                    "radius": self.last_radius,
                    "x1": pred_cx - self.last_radius,
                    "y1": pred_cy - self.last_radius,
                    "x2": pred_cx + self.last_radius,
                    "y2": pred_cy + self.last_radius,
                    "vx": self.vx,
                    "vy": self.vy,
                    "conf": max(0.2, 0.85 - 0.05 * self.missed),
                    "source": "coast",
                }
                self._draw_det(annotated, detection, coasting=True)
            else:
                self.state = f"searching ({self.missed})"
                if self.missed > self.coast_frames:
                    self._reset_track(keep_id=True)

        cv2.putText(
            annotated,
            f"FPS: {self.fps:.1f}  detect-first",
            (10, 30),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.9,
            (0, 255, 255),
            2,
        )
        color = (0, 255, 0) if self.state == "BALL" else (0, 165, 255)
        cv2.putText(
            annotated,
            self.state,
            (10, 70),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.8,
            color,
            2,
        )
        cv2.line(annotated, (w // 2, 0), (w // 2, h), (255, 0, 0), 1)
        cv2.line(annotated, (0, h // 2), (w, h // 2), (255, 0, 0), 1)
        return annotated, ([] if detection is None else [detection])

    def _choose_detection(self, glob, local, dt: float):
        """Pick ROI match near prediction, or retarget to global if ball jumped."""
        if glob is None and local is None:
            return None
        if local is not None and glob is None:
            local = dict(local)
            local["_src"] = "roi"
            return local
        if glob is not None and local is None:
            # Accept global always when unlocked; when locked only if old spot is dead
            # or the new ball is clearly better / nearby.
            if self.last_cxcy is None:
                glob = dict(glob)
                glob["_src"] = "global"
                return glob
            pred = self._predict(dt)
            dist = float(np.hypot(glob["cx"] - pred[0], glob["cy"] - pred[1]))
            gate = max(160.0, self.last_radius * 6.0 + 0.25 * float(np.hypot(self.vx, self.vy)))
            if dist <= gate or self.stale_frames >= 1:
                glob = dict(glob)
                glob["_src"] = "retarget" if dist > 80 else "global"
                return glob
            return None

        # Both available: prefer closer to prediction, but retarget if global much better.
        assert glob is not None and local is not None
        pred = self._predict(dt) if self.last_cxcy is not None else (glob["cx"], glob["cy"])
        d_local = float(np.hypot(local["cx"] - pred[0], local["cy"] - pred[1]))
        d_glob = float(np.hypot(glob["cx"] - pred[0], glob["cy"] - pred[1]))
        # If global ball is far from sticky local/tablecloth hit, take global.
        if d_glob + 40 < d_local and glob.get("mean_s", 0) >= local.get("mean_s", 0) - 20:
            glob = dict(glob)
            glob["_src"] = "retarget"
            return glob
        if local.get("score", 0) >= glob.get("score", 0) * 0.85:
            local = dict(local)
            local["_src"] = "roi"
            return local
        glob = dict(glob)
        glob["_src"] = "global"
        return glob

    def _reset_track(self, keep_id: bool = True):
        self.last_cxcy = None
        self.lock_frames = 0
        self.stale_frames = 0
        self.vx = self.vy = 0.0
        self.history.clear()
        if not keep_id:
            self.track_id += 1

    def _accept(self, cx, cy, radius, now, source: str):
        if self.last_cxcy is not None and len(self.history) >= 1:
            lx, ly, lt = self.history[-1]
            dti = max(1e-3, now - lt)
            ix, iy = (cx - lx) / dti, (cy - ly) / dti
            # Faster response when retargeting large jumps.
            jump = float(np.hypot(cx - lx, cy - ly))
            alpha = 0.55 if jump > 60 else 0.35
            self.vx = (1 - alpha) * self.vx + alpha * ix
            self.vy = (1 - alpha) * self.vy + alpha * iy
        self.last_cxcy = (float(cx), float(cy))
        self.last_radius = float(np.clip(radius, self.min_radius, self.max_radius))
        self.history.append((float(cx), float(cy), now))
        return {
            "id": self.track_id,
            "cx": float(cx),
            "cy": float(cy),
            "radius": self.last_radius,
            "x1": float(cx - self.last_radius),
            "y1": float(cy - self.last_radius),
            "x2": float(cx + self.last_radius),
            "y2": float(cy + self.last_radius),
            "vx": float(self.vx),
            "vy": float(self.vy),
            "conf": 0.95 if source != "coast" else 0.5,
            "source": source,
        }

    def _predict(self, dt: float) -> tuple[float, float]:
        if self.last_cxcy is None:
            return (0.0, 0.0)
        sp = np.hypot(self.vx, self.vy)
        if sp > 1500:
            scale = 1500 / sp
            vx, vy = self.vx * scale, self.vy * scale
        else:
            vx, vy = self.vx, self.vy
        return self.last_cxcy[0] + vx * dt, self.last_cxcy[1] + vy * dt

    def _orange_mask(self, frame, loose: bool = False):
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        lo, hi = (ORANGE_LO_LOOSE, ORANGE_HI_LOOSE) if loose else (ORANGE_LO, ORANGE_HI)
        mask = cv2.inRange(hsv, lo, hi)
        mask = cv2.medianBlur(mask, 5)
        mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, np.ones((9, 9), np.uint8))
        return mask

    def _detect_from_mask(
        self, frame, mask, gate_center=None, gate_radius=None, min_mean_s: float = MIN_MEAN_S
    ):
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        h, w = mask.shape[:2]
        m = self.border_margin
        dist = cv2.distanceTransform(mask, cv2.DIST_L2, 5)
        kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (25, 25))
        peaks = (dist == cv2.dilate(dist, kernel)) & (dist >= self.min_radius * 0.7)
        ys, xs = np.where(peaks)
        best = None
        best_score = -1.0
        for x, y in zip(xs, ys):
            r = float(dist[y, x])
            if r < self.min_radius * 0.7 or r > self.max_radius:
                continue
            # Edge glare / packaging near frame border is a common FP.
            if x < m or y < m or x >= w - m or y >= h - m:
                continue
            if gate_center is not None and gate_radius is not None:
                if np.hypot(x - gate_center[0], y - gate_center[1]) > gate_radius:
                    continue
            x0, y0 = max(0, int(x - r)), max(0, int(y - r))
            x1, y1 = min(w, int(x + r) + 1), min(h, int(y + r) + 1)
            roi_m = mask[y0:y1, x0:x1]
            roi_hsv = hsv[y0:y1, x0:x1]
            if roi_m.size == 0:
                continue
            yy, xx = np.ogrid[: roi_m.shape[0], : roi_m.shape[1]]
            cy, cx = y - y0, x - x0
            disk = (yy - cy) ** 2 + (xx - cx) ** 2 <= r * r
            if int(disk.sum()) < 30:
                continue
            fill = float(roi_m[disk].mean()) / 255.0
            if fill < 0.45:
                continue
            sats = roi_hsv[:, :, 1][disk]
            hues = roi_hsv[:, :, 0][disk]
            vals = roi_hsv[:, :, 2][disk]
            mean_s = float(sats.mean())
            mean_h = float(hues.mean())
            mean_v = float(vals.mean())
            if mean_s < min_mean_s:
                continue
            if not (8.0 <= mean_h <= 24.0):
                continue
            if mean_v < 30.0:
                continue
            # Ping-pong on table/floor is usually 20-55px here; reward that band.
            size_s = 1.0 - abs(r - 38.0) / 40.0
            size_s = float(np.clip(size_s, 0.0, 1.0))
            sat_s = float(np.clip((mean_s - 120.0) / 100.0, 0.0, 1.0))
            # Primary ranking: saturated orange disk area (rejects tiny edge noise).
            score = mean_s * fill * (0.55 + 0.45 * size_s)
            if gate_center is not None:
                dist_g = np.hypot(x - gate_center[0], y - gate_center[1])
                score *= 0.55 + 0.45 * max(0.0, 1.0 - dist_g / max(gate_radius, 1.0))
            score += 15.0 * sat_s  # mild tie-break
            if score > best_score:
                best_score = score
                best = {
                    "cx": float(x),
                    "cy": float(y),
                    "radius": float(np.clip(r * 1.15, self.min_radius, self.max_radius)),
                    "score": float(score),
                    "fill": fill,
                    "mean_s": mean_s,
                }
        return best

    def _detect_global(self, frame):
        mask = self._orange_mask(frame, loose=False)
        hit = self._detect_from_mask(frame, mask, min_mean_s=MIN_MEAN_S)
        if hit is None:
            mask = self._orange_mask(frame, loose=True)
            hit = self._detect_from_mask(frame, mask, min_mean_s=MIN_MEAN_S_LOOSE)
        return hit

    def _detect_in_roi(self, frame, center, radius_hint: float, expand: float | None = None):
        h, w = frame.shape[:2]
        cx, cy = center
        exp = self.reacquire_expand if expand is None else expand
        rad = max(120.0, radius_hint * exp * (1.0 + 0.12 * self.missed))
        x0 = max(0, int(cx - rad))
        y0 = max(0, int(cy - rad))
        x1 = min(w, int(cx + rad))
        y1 = min(h, int(cy + rad))
        if x1 - x0 < 20 or y1 - y0 < 20:
            return None
        crop = frame[y0:y1, x0:x1]
        mask = self._orange_mask(crop, loose=True)
        hit = self._detect_from_mask(
            crop,
            mask,
            gate_center=(cx - x0, cy - y0),
            gate_radius=rad,
            min_mean_s=MIN_MEAN_S_LOOSE,
        )
        if hit is None:
            return None
        hit["cx"] += x0
        hit["cy"] += y0
        return hit

    def _orange_quality(self, frame, cx, cy, radius) -> dict:
        h, w = frame.shape[:2]
        r = max(8, int(radius))
        x0, y0 = max(0, int(cx - r)), max(0, int(cy - r))
        x1, y1 = min(w, int(cx + r)), min(h, int(cy + r))
        if x1 <= x0 or y1 <= y0:
            return {"mean_s": 0.0, "fill": 0.0}
        patch = frame[y0:y1, x0:x1]
        hsv = cv2.cvtColor(patch, cv2.COLOR_BGR2HSV)
        mask = cv2.inRange(hsv, ORANGE_LO, ORANGE_HI)
        fill = float(np.count_nonzero(mask)) / float(mask.size)
        mean_s = float(hsv[:, :, 1][mask > 0].mean()) if np.any(mask) else 0.0
        return {"mean_s": mean_s, "fill": fill}

    def _draw_det(self, annotated, det, coasting: bool = False):
        cx, cy = det["cx"], det["cy"]
        r = int(max(10, det.get("radius", 20)))
        color = (0, 255, 255) if coasting else (0, 255, 0)
        cv2.circle(annotated, (int(cx), int(cy)), r, color, 3)
        cv2.circle(annotated, (int(cx), int(cy)), 4, (0, 0, 255), -1)
        tag = "BALL~" if coasting else "BALL"
        label = f"{tag} ID:{det['id']} {det.get('source', '')}"
        cv2.putText(
            annotated,
            label,
            (int(cx - r), max(28, int(cy - r - 10))),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.8,
            color,
            2,
        )
        self._draw_trajectory(annotated)
        vx, vy = det.get("vx", 0.0), det.get("vy", 0.0)
        if abs(vx) > 5 or abs(vy) > 5:
            end = (
                int(cx + np.clip(vx * 0.05, -140, 140)),
                int(cy + np.clip(vy * 0.05, -140, 140)),
            )
            cv2.arrowedLine(annotated, (int(cx), int(cy)), end, (255, 0, 0), 2)

    def _draw_trajectory(self, frame):
        if len(self.history) < 2:
            return
        pts = list(self.history)
        for i in range(1, len(pts)):
            p1 = (int(pts[i - 1][0]), int(pts[i - 1][1]))
            p2 = (int(pts[i][0]), int(pts[i][1]))
            alpha = i / len(pts)
            color = (0, int(255 * alpha), int(255 * (1 - alpha)))
            cv2.line(frame, p1, p2, color, 2)


def open_camera(index: int, width: int, height: int, fps: int | None = None):
    cap = cv2.VideoCapture(index, cv2.CAP_AVFOUNDATION)
    if not cap.isOpened():
        cap = cv2.VideoCapture(index)
    if not cap.isOpened():
        return None

    cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
    if fps is not None:
        cap.set(cv2.CAP_PROP_FPS, fps)
    # Try to lift underexposure on AR0234 (best-effort; drivers differ).
    try:
        cap.set(cv2.CAP_PROP_AUTO_EXPOSURE, 0.75)  # auto on some backends
        cap.set(cv2.CAP_PROP_GAIN, 40)
    except Exception:
        pass

    for _ in range(20):
        ret, _ = cap.read()
        if ret:
            return cap
        time.sleep(0.03)
    cap.release()
    return None


def parse_args():
    p = argparse.ArgumentParser(description="Continuous orange ball tracker")
    p.add_argument("--camera", type=int, default=0)
    p.add_argument("--width", type=int, default=1920)
    p.add_argument("--height", type=int, default=1200)
    p.add_argument("--fps", type=int, default=60)
    p.add_argument("--headless", action="store_true")
    p.add_argument("--duration", type=float, default=0.0)
    p.add_argument("--out-dir", type=Path, default=Path("runs/live_track"))
    return p.parse_args()


def main():
    args = parse_args()
    tracker = BallTracker()
    cap = open_camera(args.camera, args.width, args.height, args.fps)
    if cap is None:
        print("ERROR: 无法打开摄像头。检查权限 / 是否被占用。")
        sys.exit(1)

    aw = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    ah = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    print(f"摄像头 index={args.camera} ({aw}x{ah}) detect-first tracking")
    print("橙色检测优先；球离开旧点会立刻放掉并重锁。ESC 退出")

    args.out_dir.mkdir(parents=True, exist_ok=True)
    writer = None
    if args.headless:
        writer = cv2.VideoWriter(
            str(args.out_dir / "track_live.mp4"),
            cv2.VideoWriter_fourcc(*"mp4v"),
            15.0,
            (aw, ah),
        )

    t0 = time.time()
    frame_i = 0
    try:
        while True:
            ret, frame = cap.read()
            if not ret:
                print("读帧失败")
                break
            annotated, detections = tracker.process_frame(frame)
            if detections:
                d = detections[0]
                print(
                    f"{tracker.state} 位置:({d['cx']:.0f},{d['cy']:.0f}) "
                    f"v=({d['vx']:.0f},{d['vy']:.0f}) src={d.get('source')}",
                    flush=True,
                )
            if writer is not None:
                writer.write(annotated)
            if args.headless and frame_i % 20 == 0:
                path = args.out_dir / f"live_{frame_i:05d}.jpg"
                cv2.imwrite(str(path), annotated)
            if not args.headless:
                cv2.imshow("Ball Tracker - AR0234", annotated)
                key = cv2.waitKey(1) & 0xFF
                if key == 27:
                    break
                if key == ord("s"):
                    out = args.out_dir / f"capture_{int(time.time())}.jpg"
                    cv2.imwrite(str(out), annotated)
                    print(f"已保存 {out}")
            frame_i += 1
            if args.duration > 0 and time.time() - t0 >= args.duration:
                break
    except KeyboardInterrupt:
        print("\n中断退出")
    finally:
        cap.release()
        if writer is not None:
            writer.release()
        if not args.headless:
            cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
