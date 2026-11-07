---
name: code-annotate-and-commit
description: >-
  Requires Chinese logic comments on every API/method call (including English
  third-party libraries) and outputs Conventional Commit messages like
  feat:小球算法优化. Supports any commit type. Use when writing or editing code,
  wrapping library APIs, finishing a coding task, committing, or when the user
  asks for 备注、注释、commit 说明、提交说明.
---

# 代码备注 + Git Commit 说明

后续所有 AI 写代码、改代码、封装调用时必须遵守本 skill。

## 必须：接口与方法备注

必须对所有调用接口和方法备注逻辑说明，特别是有些英语第三方库调用方法也要说明调用这个的作用。

### 备注范围

- 自写函数 / 类 / 方法：用途、入参含义、返回值、关键副作用
- 项目内互相调用：为何调用、输入输出约定
- **第三方英文库**（OpenCV / NumPy / ultralytics / torch / ROS2 等）：在调用旁用中文说明「调用这个的作用」
- HTTP / RPC / 系统 API：请求意图、关键参数、失败影响

### 写法

- 语言：**中文**（专有名词可保留英文）
- 位置：定义上方 docstring，或调用行正上方 `#` 注释
- 内容：说明**逻辑作用**，不要复述英文函数名
- 禁止：空注释、`# call foo`、与代码无关的废话

### 示例

```python
# 打开 AR0234 UVC 摄像头；CAP_AVFOUNDATION 是 macOS 可用的采集后端
cap = cv2.VideoCapture(index, cv2.CAP_AVFOUNDATION)

# 按橙色 HSV 范围生成二值掩膜，用于从画面中抠出小球候选区域
mask = cv2.inRange(hsv, ORANGE_LO, ORANGE_HI)

# 距离变换：掩膜内像素到背景的距离，峰值近似圆盘中心（球心）
dist = cv2.distanceTransform(mask, cv2.DIST_L2, 5)
```

```python
def process_frame(self, frame):
    """处理单帧：检测橙色小球并更新轨迹，返回标注图与检测列表。"""
```

### 完成前自检

- [ ] 新增/修改的自写接口有中文逻辑说明
- [ ] 每个第三方英文库调用旁有「作用」说明
- [ ] 注释解释为什么调用，而不是翻译函数名

---

## 必须：输出 Git Commit 说明

每次完成可提交的代码改动后，**在回复中输出一条 commit 说明**（即使用户暂未要求执行 `git commit`）。

### 格式

```text
<type>:<中文摘要>
```

**能支持任何类型 git 提交**：`type` 不限制死枚举。常用推荐：

| type | 用途 |
|------|------|
| `feat` | 新功能 |
| `fix` | 缺陷修复 |
| `refactor` | 重构（无行为变化） |
| `perf` | 性能优化 |
| `docs` | 文档 |
| `style` | 格式/空格（无逻辑变化） |
| `test` | 测试 |
| `build` | 构建 / 依赖 |
| `ci` | CI 配置 |
| `chore` | 杂项维护 |
| `revert` | 回滚 |

也允许项目自定义 type（如 `wip`、`release`、`data`），只要是小写英文单词 + 冒号 + 中文摘要。

- 冒号后直接跟中文，**无空格也可**（与用户示例一致）
- 摘要短、说清「为什么 / 做了什么」
- 可选 scope：`feat(tracker):橙色球连续跟踪`

### 示例

```text
feat:小球算法优化
```

```text
fix:修复球离开后跟踪粘在旧位置
```

```text
docs:补充数据集与训练说明
```

```text
chore:忽略本地摄像头调试截图
```

```text
data:扩充小球训练集标注
```

```text
ci:添加训练流水线
```

### 用户要求提交时

1. 先给出上述格式的 commit 说明（按改动选任意合适 type）
2. 再用该说明执行 `git commit`（遵循仓库 git 安全规则：不改 git config、不强制推送等）
3. 用户明确要求推送时再 `git push`
4. **任何类型的改动**（功能 / 修复 / 文档 / 数据 / CI / 杂项）都可以提交，不要因 type 不在常用表而拒绝提交

---

## 本仓库说明

本 skill 位于 `.cursor/skills/`，与个人全局 skill 同步约束本仓库 AI 改动。
