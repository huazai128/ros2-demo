# lidar — LD14 / LD14P 激光雷达

只维护雷达串口驱动与可视化，与 `../camera` 解耦。

## 结构

```text
lidar/
├── lichtblick/ld14_laserscan.json   # Lichtblick 3D 布局（含 /scan）
└── scripts/
    ├── run_ld14_viewer.sh           # 驱动 + RViz2
    ├── run_ld14_lichtblick.sh       # 驱动 + foxglove_bridge + Lichtblick
    └── probe_scan.py                # 统计 /scan 中 NaN / 有效点
```

## 硬件约定

| 项 | 值 |
|----|-----|
| 实测型号 | LD14P |
| 串口 | 多为 `/dev/ttyACM0`（QinHeng `1a86:55d4`） |
| 波特率 | `230400` |
| 话题 | `/scan` |
| 驱动工作空间 | `~/ldlidar_ros2_ws` |

`/scan` 里无效角度为 **NaN**（官方驱动惯例），可视化会忽略。

## 使用

```bash
cd /home/hua/Downloads/project/ros2-demo

bash lidar/scripts/run_ld14_viewer.sh
bash lidar/scripts/run_ld14_lichtblick.sh

source /opt/ros/humble/setup.bash
source ~/ldlidar_ros2_ws/install/setup.bash
python3 lidar/scripts/probe_scan.py
```

串口权限：`sudo usermod -aG dialout $USER` 后重新登录。
