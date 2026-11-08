#!/usr/bin/env bash
# 启动 LD14P + foxglove_bridge + Lichtblick 3D（ros2-demo/lidar 子目录）
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
WS="${LDLIDAR_WS:-${HOME}/ldlidar_ros2_ws}"
LICHTBLICK_BIN="${LICHTBLICK_BIN:-${HOME}/lichtblick-app/lichtblick-1.30.0-linux-arm64/lichtblick}"
LAYOUT="${ROOT}/lichtblick/ld14_laserscan.json"

detect_port() {
  if [[ "${1:-}" != "" ]]; then
    echo "$1"
    return
  fi
  local p
  for p in /dev/serial/by-id/* /dev/ttyACM* /dev/ttyUSB*; do
    if [[ -e "$p" ]]; then
      echo "$p"
      return
    fi
  done
  return 1
}

# source：加载 ROS2 与 LD14 包环境
# shellcheck disable=SC1091
source /opt/ros/humble/setup.bash
# shellcheck disable=SC1091
source "${WS}/install/setup.bash"

if ! PORT="$(detect_port "${1:-}")"; then
  echo "未找到雷达串口，请先把 LD14 USB 接到虚拟机"
  exit 1
fi

if [[ ! -r "${PORT}" || ! -w "${PORT}" ]]; then
  echo "串口无权限: ${PORT}，尝试 chmod..."
  # pkexec：图形授权临时放开串口读写
  pkexec chmod 666 "${PORT}" || true
fi

export DISPLAY="${DISPLAY:-:0}"

# 若 /scan 尚未发布，则启动 LD14P 驱动（不带 RViz，交给 Lichtblick）
if ! timeout 2 ros2 topic hz /scan >/tmp/ld14_hz_check.log 2>&1; then
  echo "启动 LD14P 驱动，串口=${PORT}"
  # ros2 launch：发布 /scan 与 base_link→base_laser 静态 TF
  nohup ros2 launch ldlidar_sl_ros2 ld14p.launch.py port_name:="${PORT}" \
    >/tmp/ld14p_node.log 2>&1 &
  sleep 3
fi

# foxglove_bridge：把 ROS2 话题桥到 WebSocket，供 Lichtblick 拉取
if ! ss -ltn 2>/dev/null | grep -q ':8765 '; then
  echo "启动 foxglove_bridge @ ws://localhost:8765"
  nohup ros2 launch foxglove_bridge foxglove_bridge_launch.xml \
    >/tmp/foxglove_bridge.log 2>&1 &
  sleep 2
fi

if [[ ! -x "${LICHTBLICK_BIN}" ]]; then
  echo "找不到 Lichtblick: ${LICHTBLICK_BIN}"
  echo "请安装到该路径，或 export LICHTBLICK_BIN=/path/to/lichtblick"
  exit 1
fi

# http.server：把布局 JSON 挂到本机 8766，供 deep link 的 layoutUrl 下载
if ! ss -ltn | grep -q ':8766 '; then
  (cd "$(dirname "${LAYOUT}")" && python3 -m http.server 8766 --bind 127.0.0.1 \
    >/tmp/lichtblick_layout_http.log 2>&1) &
  sleep 1
fi

echo "打开 Lichtblick 并自动连接 ws://localhost:8765 + LD14 布局..."
# Electron 启动参数：VMware 无硬件 3D 时用 SwiftShader 软渲染 WebGL
nohup "${LICHTBLICK_BIN}" \
  --no-sandbox \
  --ignore-gpu-blocklist \
  --enable-webgl \
  --enable-unsafe-swiftshader \
  --use-gl=angle \
  --use-angle=swiftshader-webgl \
  --disable-gpu-sandbox \
  >/tmp/lichtblick.log 2>&1 &
sleep 2

# xdg-open：通过 lichtblick:// 协议让已运行实例加载数据源与布局
DEEP="lichtblick://open?ds=foxglove-websocket&ds.url=ws%3A%2F%2Flocalhost%3A8765&layoutUrl=http%3A%2F%2F127.0.0.1%3A8766%2Fld14_laserscan.json"
xdg-open "${DEEP}" >/tmp/xdg_lichtblick.log 2>&1 || true

echo "已请求连接。若 3D 无点云：Topics 勾选 /scan，Color=Flat 绿色"
echo "布局文件: ${LAYOUT}"
