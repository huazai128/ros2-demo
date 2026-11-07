"""Validate YOLO ball annotations and copy deterministic train/val splits."""
import argparse
import hashlib
import json
import math
from pathlib import Path
import random
import shutil

ROOT = Path(__file__).resolve().parents[1]
EXTENSIONS = {'.jpg', '.jpeg', '.png', '.bmp', '.webp', '.tif', '.tiff'}


def collect(source):
    images = sorted(p for p in source.rglob('*') if p.suffix.lower() in EXTENSIONS)
    if len(images) < 2:
        raise ValueError('至少需要两张图片才能划分训练集和验证集')
    stems, hashes, pairs = set(), set(), []
    for image in images:
        if image.stem.casefold() in stems:
            raise ValueError(f'存在重名图片: {image.name}')
        stems.add(image.stem.casefold())
        digest = hashlib.sha256(image.read_bytes()).hexdigest()
        if digest in hashes:
            raise ValueError(f'存在内容重复的图片，可能导致训练/验证泄漏: {image}')
        hashes.add(digest)
        label = image.with_suffix('.txt')
        if not label.is_file():
            raise ValueError(f'缺少标签: {label}')
        # An empty file explicitly represents a background image.
        for line_no, line in enumerate(label.read_text().splitlines(), 1):
            if not line.strip():
                continue
            fields = line.split()
            try:
                if len(fields) != 5 or fields[0] != '0':
                    raise ValueError()
                x, y, w, h = map(float, fields[1:])
                if not all(math.isfinite(v) for v in (x, y, w, h)):
                    raise ValueError()
                if not (0 <= x <= 1 and 0 <= y <= 1 and 0 < w <= 1 and 0 < h <= 1):
                    raise ValueError()
                if min(x-w/2, y-h/2) < -1e-6 or max(x+w/2, y+h/2) > 1+1e-6:
                    raise ValueError()
            except ValueError:
                raise ValueError(f'无效 YOLO 标签 {label}:{line_no}: {line}') from None
        pairs.append((image, label))
    orphans = sorted(p for p in source.rglob('*.txt') if p.stem.casefold() not in stems)
    if orphans:
        raise ValueError(f'存在无对应图片的标签: {orphans}')
    return pairs


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, required=True)
    parser.add_argument('--output', type=Path, default=ROOT / 'ball_dataset')
    parser.add_argument('--train-ratio', type=float, default=0.7)
    parser.add_argument('--seed', type=int, default=42)
    args = parser.parse_args()
    source, output = args.source.resolve(), args.output.resolve()
    if not source.is_dir():
        parser.error(f'源目录不存在: {source}')
    if source == output or source in output.parents or output in source.parents:
        parser.error('源目录和输出目录不能互相包含')
    if not 0 < args.train_ratio < 1:
        parser.error('--train-ratio 必须在 0 和 1 之间')
    pairs = collect(source)
    random.Random(args.seed).shuffle(pairs)
    count = max(1, min(len(pairs)-1, int(len(pairs)*args.train_ratio+0.5)))
    splits = {'train': pairs[:count], 'val': pairs[count:]}
    # Refuse stale or unrelated files instead of deleting user data on reruns.
    expected = {}
    for split, items in splits.items():
        for image, label in items:
            expected[output / 'images' / split / image.name] = image
            expected[output / 'labels' / split / label.name] = label
    for kind in ('images', 'labels'):
        folder = output / kind
        if folder.exists():
            for existing in folder.rglob('*'):
                if existing.is_file() and existing not in expected:
                    raise ValueError(f'输出目录包含旧文件，请使用新的 --output: {existing}')
    for destination, original in expected.items():
        if destination.exists() and destination.read_bytes() != original.read_bytes():
            raise ValueError(f'拒绝覆盖内容不同的文件: {destination}')
    for destination, original in expected.items():
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(original, destination)
    # Use an absolute dataset root so training works from any working directory.
    (output / 'data.yaml').write_text(f'path: {json.dumps(str(output))}\ntrain: images/train\nval: images/val\n\nnames:\n  0: ball\n')
    manifest = {'source': str(source), 'seed': args.seed, 'train_ratio': args.train_ratio,
                'splits': {split: [image.name for image, _ in items] for split, items in splits.items()}}
    (output / 'split_manifest.json').write_text(json.dumps(manifest, ensure_ascii=False, indent=2)+'\n')
    print(f'已生成 {output}: train={count}, val={len(pairs)-count}')


if __name__ == '__main__':
    main()
