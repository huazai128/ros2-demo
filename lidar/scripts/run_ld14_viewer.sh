#!/usr/bin/env bash
# RViz2 查看 LD14P 实时 LaserScan（ros2-demo/lidar 子目录入口）
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
WS="${LDLIDAR_WS:-${HOME}/ldlidar_ros2_ws}"

detect_port() {
  # 优先使用命令行传入的串口；否则自动探测 ACM/USB
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

# source：加载 ROS2 Humble 与本机编译的 ldlidar_sl_ros2
# shellcheck disable=SC1091
source /opt/ros/humble/setup.bash
# shellcheck disable=SC1091
source "${WS}/install/setup.bash"

if ! PORT="$(detect_port "${1:-}")"; then
  echo "未找到雷达串口。请把 LD14 USB 接到本机/虚拟机后重试。"
  exit 1
fi

echo "串口=${PORT} 布局项目=${ROOT}"

if [[ ! -r "${PORT}" || ! -w "${PORT}" ]]; then
  echo "串口无读写权限，尝试图形授权 chmod..."
  # pkexec：弹出授权框，临时放开串口（长期请加入 dialout 组）
  pkexec chmod 666 "${PORT}" || {
    echo "请执行: sudo usermod -aG dialout \$USER 后重新登录"
    exit 1
  }
fi

export DISPLAY="${DISPLAY:-:0}"

# ros2 launch：驱动发布 /scan，并打开官方 RViz 配置
# 本机实测为 LD14P@230400；若是真 LD14 请改 viewer_ld14.launch.py
exec ros2 launch ldlidar_sl_ros2 viewer_ld14p.launch.py port_name:="${PORT}"
