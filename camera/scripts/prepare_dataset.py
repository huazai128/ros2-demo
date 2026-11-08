"""校验 YOLO 小球标注，并按固定种子划分、复制 train/val 数据集。"""
import argparse
import hashlib
import json
import math
from pathlib import Path
import random
import shutil

# 仓库根目录（scripts/ 的上一级）
ROOT = Path(__file__).resolve().parents[1]
# 允许作为训练图片的后缀集合
EXTENSIONS = {'.jpg', '.jpeg', '.png', '.bmp', '.webp', '.tif', '.tiff'}


def collect(source):
    """扫描源目录，校验图片-标签配对与 YOLO 框合法性，返回 (图片, 标签) 列表。

    入参 source: 含图片与同名 .txt 标签的目录。
    返回: [(image_path, label_path), ...]
    失败时抛 ValueError，避免脏数据进入训练。
    """
    # rglob：递归找出所有候选图片路径，再按后缀过滤并排序，保证划分可复现
    images = sorted(p for p in source.rglob('*') if p.suffix.lower() in EXTENSIONS)
    if len(images) < 2:
        raise ValueError('至少需要两张图片才能划分训练集和验证集')
    stems, hashes, pairs = set(), set(), []
    for image in images:
        if image.stem.casefold() in stems:
            raise ValueError(f'存在重名图片: {image.name}')
        stems.add(image.stem.casefold())
        # sha256：用文件内容哈希检测重复图，防止同一张图同时进 train/val 造成泄漏
        digest = hashlib.sha256(image.read_bytes()).hexdigest()
        if digest in hashes:
            raise ValueError(f'存在内容重复的图片，可能导致训练/验证泄漏: {image}')
        hashes.add(digest)
        # with_suffix：把图片路径改成同名 .txt 标签路径
        label = image.with_suffix('.txt')
        if not label.is_file():
            raise ValueError(f'缺少标签: {label}')
        # 空标签文件表示背景图（无目标），这是合法的负样本
        for line_no, line in enumerate(label.read_text().splitlines(), 1):
            if not line.strip():
                continue
            fields = line.split()
            try:
                # YOLO 格式：class cx cy w h（归一化到 0~1），本任务类别固定为 0=ball
                if len(fields) != 5 or fields[0] != '0':
                    raise ValueError()
                x, y, w, h = map(float, fields[1:])
                # math.isfinite：拒绝 NaN/Inf，避免训练崩溃
                if not all(math.isfinite(v) for v in (x, y, w, h)):
                    raise ValueError()
                if not (0 <= x <= 1 and 0 <= y <= 1 and 0 < w <= 1 and 0 < h <= 1):
                    raise ValueError()
                # 框中心±半宽高不得明显越出 [0,1] 图像范围
                if min(x - w / 2, y - h / 2) < -1e-6 or max(x + w / 2, y + h / 2) > 1 + 1e-6:
                    raise ValueError()
            except ValueError:
                raise ValueError(f'无效 YOLO 标签 {label}:{line_no}: {line}') from None
        pairs.append((image, label))
    # 找出没有对应图片的孤儿标签，避免静默丢数据
    orphans = sorted(p for p in source.rglob('*.txt') if p.stem.casefold() not in stems)
    if orphans:
        raise ValueError(f'存在无对应图片的标签: {orphans}')
    return pairs


def main():
    """解析参数，划分 train/val，安全复制到输出目录并写 data.yaml / manifest。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, required=True)
    parser.add_argument('--output', type=Path, default=ROOT / 'ball_dataset')
    parser.add_argument('--train-ratio', type=float, default=0.7)
    parser.add_argument('--seed', type=int, default=42)
    args = parser.parse_args()
    # resolve：转成绝对路径，避免相对路径随 cwd 变化
    source, output = args.source.resolve(), args.output.resolve()
    if not source.is_dir():
        parser.error(f'源目录不存在: {source}')
    if source == output or source in output.parents or output in source.parents:
        parser.error('源目录和输出目录不能互相包含')
    if not 0 < args.train_ratio < 1:
        parser.error('--train-ratio 必须在 0 和 1 之间')

    pairs = collect(source)
    # Random(seed).shuffle：固定种子打乱，保证每次划分结果一致
    random.Random(args.seed).shuffle(pairs)
    # 四舍五入得到训练集数量，并保证 train/val 都至少 1 张
    count = max(1, min(len(pairs) - 1, int(len(pairs) * args.train_ratio + 0.5)))
    splits = {'train': pairs[:count], 'val': pairs[count:]}

    # 拒绝静默删除用户数据：若输出目录有本次不该存在的旧文件则报错
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
        # 目标已存在且内容不同则拒绝覆盖，防止误伤手工改过的文件
        if destination.exists() and destination.read_bytes() != original.read_bytes():
            raise ValueError(f'拒绝覆盖内容不同的文件: {destination}')
    for destination, original in expected.items():
        # mkdir(parents=True)：自动创建 images/train 等中间目录
        destination.parent.mkdir(parents=True, exist_ok=True)
        # copy2：复制文件并尽量保留时间戳等元数据
        shutil.copy2(original, destination)

    # 写 Ultralytics 数据配置；path 用绝对路径，任意 cwd 下训练都能找到数据
    # json.dumps：把路径安全转成 YAML 可用的带引号字符串
    (output / 'data.yaml').write_text(
        f'path: {json.dumps(str(output))}\n'
        f'train: images/train\nval: images/val\n\nnames:\n  0: ball\n'
    )
    # 记录划分清单，方便复现实验与核对文件名
    manifest = {
        'source': str(source),
        'seed': args.seed,
        'train_ratio': args.train_ratio,
        'splits': {
            split: [image.name for image, _ in items]
            for split, items in splits.items()
        },
    }
    # ensure_ascii=False：中文路径/说明原样写入；indent=2 便于人工阅读
    (output / 'split_manifest.json').write_text(
        json.dumps(manifest, ensure_ascii=False, indent=2) + '\n'
    )
    print(f'已生成 {output}: train={count}, val={len(pairs) - count}')


if __name__ == '__main__':
    main()
