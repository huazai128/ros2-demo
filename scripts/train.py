"""Train a ball detector on Apple MPS, with CPU fallback when unavailable."""
import argparse
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--data', type=Path, default=ROOT / 'ball_dataset/data.yaml')
    parser.add_argument('--model', default='yolo11n.pt')
    parser.add_argument('--epochs', type=int, default=100)
    parser.add_argument('--imgsz', type=int, default=640)
    parser.add_argument('--batch', type=int, default=4)
    parser.add_argument('--device', choices=['auto', 'mps', 'cpu'], default='auto')
    args = parser.parse_args()
    if not args.data.is_file():
        parser.error(f'数据配置不存在: {args.data}')
    if min(args.epochs, args.imgsz, args.batch) <= 0:
        parser.error('epochs、imgsz、batch 必须为正整数')
    import torch
    from ultralytics import YOLO
    device = args.device
    if device == 'auto':
        device = 'mps' if torch.backends.mps.is_available() else 'cpu'
    if device == 'mps' and not torch.backends.mps.is_available():
        parser.error('当前 PyTorch 环境不支持 MPS，请使用 --device cpu')
    print(f'训练设备: {device}')
    YOLO(args.model).train(
        data=str(args.data.resolve()), epochs=args.epochs, imgsz=args.imgsz,
        batch=args.batch, device=device, workers=0, seed=42,
        project=str(ROOT / 'runs/detect'), name='ball',
    )


if __name__ == '__main__':
    main()
