# YOLO 小球检测（Mac M2）

## 数据准备

已从 `/Users/hua/Downloads/project/ros2/labels/images_to_label` 复制 11 张图片和同名标签，训练集 8 张、验证集 3 张（整数划分约 70% / 30%）。保留原文件，类别 `0: ball`，原有框坐标保持不变。

```text
ball_dataset/
├── images/{train,val}/
├── labels/{train,val}/
├── data.yaml
└── split_manifest.json
```

在项目目录运行，重新生成相同划分：

```bash
python3 scripts/prepare_dataset.py --source /Users/hua/Downloads/project/ros2/labels/images_to_label
```

脚本固定随机种子 42，检查图片与标签配对、重名、重复图片、类别、坐标范围和框边界；空标签表示背景图片。输出中的旧文件或内容冲突会报错，不自动删除。更改种子、比例或源数据时可用 `--output ball_dataset_v2` 写入新目录。数据集移动后应重新生成配置中的绝对路径。

## 安装和训练

```bash
python3 -m venv .venv
source .venv/bin/activate
python -m pip install -r requirements.txt
python scripts/train.py
```

训练入口优先选择 MPS，不可用时使用 CPU；默认 YOLO11n、100 轮、640 像素、batch 4、workers 0。首次训练会下载预训练权重，需要联网。结果保存在 `runs/detect/ball*`，最佳权重在对应目录的 `weights/best.pt`。

```bash
python scripts/train.py --epochs 20 --batch 2 --device mps
```

数据仅 11 张，适合验证流程；尤其是相邻帧较相似时，随机划分的验证指标不能代表真实场景效果，后续应补充不同场景并按采集序列划分。

MPS 参数参考 [Ultralytics 训练文档](https://docs.ultralytics.com/modes/train/)。
