# camera — 摄像头橙色小球跟踪

只维护视觉相关代码，与 `../lidar` 解耦。

## 结构

```text
camera/
├── ball_tracker.py      # 实时橙色球检测优先跟踪
├── ball_dataset/        # YOLO 数据与 data.yaml
├── requirements.txt
└── scripts/
    ├── prepare_dataset.py
    └── train.py
```

## 使用

在仓库根目录或本目录执行均可；下面以仓库根为例：

```bash
cd /home/hua/Downloads/project/ros2-demo

python3 -m venv camera/.venv
source camera/.venv/bin/activate
python -m pip install -r camera/requirements.txt

# 划分数据集（按需改 --source）
python camera/scripts/prepare_dataset.py --source /path/to/images_to_label

# 训练
python camera/scripts/train.py

# 实时跟踪
python camera/ball_tracker.py --camera 0
```

## 备注规范

第三方库（OpenCV / NumPy / Ultralytics 等）调用旁须写中文「作用」说明，见仓库 skill。
