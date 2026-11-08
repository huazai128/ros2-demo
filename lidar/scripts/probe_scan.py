#!/usr/bin/env python3
"""订阅 /scan，统计 LaserScan 中 NaN / 有效距离，用于确认雷达数据是否正常。"""

from __future__ import annotations

import math
import statistics
import time

import rclpy
from sensor_msgs.msg import LaserScan


def summarize(msg: LaserScan) -> None:
    """打印一帧扫描的点数、NaN 比例与有效距离范围。

    msg: 官方 LD14P 节点发布的 LaserScan；无效回波通常为 quiet_NaN。
    """
    # 收集有限距离，供 min/max/mean；NaN/Inf 单独计数
    finite = [r for r in msg.ranges if math.isfinite(r)]
    nan_n = sum(1 for r in msg.ranges if math.isnan(r))
    inf_n = sum(1 for r in msg.ranges if math.isinf(r))
    # intensities 与 ranges 对齐；无效点强度也常为 NaN
    inten = list(msg.intensities)
    inten_finite = [v for v in inten if math.isfinite(v)]

    print(f"frame={msg.header.frame_id} points={len(msg.ranges)}")
    print(f"ranges: finite={len(finite)} nan={nan_n} inf={inf_n}")
    if finite:
        print(
            f"range: min={min(finite):.3f} max={max(finite):.3f} "
            f"mean={statistics.mean(finite):.3f} m"
        )
    print(f"intensities: finite={len(inten_finite)} / {len(inten)}")
    # NaN 是官方驱动对「无回波」的表示，不是通信错误
    if nan_n:
        pct = 100.0 * nan_n / max(len(msg.ranges), 1)
        print(f"说明: NaN≈{pct:.1f}% 为无效角度，可视化会忽略这些点")


def main() -> None:
    """初始化 ROS2 节点，等待一帧 /scan 后打印统计并退出。"""
    # rclpy.init：初始化 ROS2 客户端上下文
    rclpy.init()
    # create_node：诊断用临时节点，不常驻
    node = rclpy.create_node("ld14_probe_scan")
    box: dict[str, LaserScan] = {}

    def on_scan(msg: LaserScan) -> None:
        """收到首帧扫描后缓存，主循环据此退出。"""
        box["msg"] = msg

    # create_subscription：订阅雷达 LaserScan；QoS 默认即可对接官方驱动
    node.create_subscription(LaserScan, "/scan", on_scan, 10)

    t0 = time.time()
    # spin_once：单次处理回调，避免阻塞式 spin 难退出
    while "msg" not in box and time.time() - t0 < 8.0:
        rclpy.spin_once(node, timeout_sec=0.2)

    if "msg" not in box:
        print("ERROR: 8 秒内未收到 /scan。请先启动 LD14P 驱动。")
        # destroy_node / shutdown：释放节点与全局上下文
        node.destroy_node()
        rclpy.shutdown()
        raise SystemExit(1)

    summarize(box["msg"])
    node.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()
