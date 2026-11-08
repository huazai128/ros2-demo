"""在 Apple MPS（可用时）或 CPU 上训练小球 YOLO 检测器。"""
import argparse
from pathlib import Path

# 定位到仓库根目录（本文件在 scripts/ 下，上溯一级）
ROOT = Path(__file__).resolve().parents[1]


def main():
    """解析训练参数，选择设备，启动 Ultralytics YOLO 训练。"""
    # 构建命令行参数解析器；description 复用模块文档字符串
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--data', type=Path, default=ROOT / 'ball_dataset/data.yaml')
    parser.add_argument('--model', default='yolo11n.pt')
    parser.add_argument('--epochs', type=int, default=100)
    parser.add_argument('--imgsz', type=int, default=640)
    parser.add_argument('--batch', type=int, default=4)
    parser.add_argument('--device', choices=['auto', 'mps', 'cpu'], default='auto')
    # 解析用户传入的 CLI 参数
    args = parser.parse_args()
    if not args.data.is_file():
        parser.error(f'数据配置不存在: {args.data}')
    if min(args.epochs, args.imgsz, args.batch) <= 0:
        parser.error('epochs、imgsz、batch 必须为正整数')

    # torch：判断本机是否支持 Apple Metal（MPS）加速
    import torch
    # ultralytics.YOLO：加载预训练权重并执行目标检测训练
    from ultralytics import YOLO

    device = args.device
    if device == 'auto':
        # backends.mps.is_available：检测当前 PyTorch 能否用 MPS，否则退回 CPU
        device = 'mps' if torch.backends.mps.is_available() else 'cpu'
    if device == 'mps' and not torch.backends.mps.is_available():
        parser.error('当前 PyTorch 环境不支持 MPS，请使用 --device cpu')
    print(f'训练设备: {device}')

    # YOLO(...).train：按 data.yaml 微调检测头，结果写入 runs/detect/ball*
    YOLO(args.model).train(
        data=str(args.data.resolve()), epochs=args.epochs, imgsz=args.imgsz,
        batch=args.batch, device=device, workers=0, seed=42,
        project=str(ROOT / 'runs/detect'), name='ball',
    )


if __name__ == '__main__':
    main()
