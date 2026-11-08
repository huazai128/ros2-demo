# ros2-demo

一个仓库，**摄像头**与**激光雷达**分目录单独维护，互不 import、不共用启动脚本。

```text
ros2-demo/
├── camera/          # 橙色小球检测跟踪、数据集、YOLO 训练
├── lidar/           # LD14P 串口驱动启动、RViz / Lichtblick
└── .cursor/skills/  # 中文备注 + Conventional Commit 规范
```

| 目录 | 做什么 | 入口 |
|------|--------|------|
| `camera/` | AR0234 / OpenCV 跟踪、训练 | `python3 camera/ball_tracker.py` |
| `lidar/` | LD14P `/scan` 可视化 | `bash lidar/scripts/run_ld14_viewer.sh` |

详细说明见各子目录 README：

- [camera/README.md](camera/README.md)
- [lidar/README.md](lidar/README.md)

改代码时遵守 `.cursor/skills/code-annotate-and-commit/SKILL.md`（中文逻辑备注 + `feat:…` 式 commit 说明）。
