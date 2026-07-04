# 从零跑起来 — IslandGrid UE5 项目安装步骤

本目录已是一个可打开的 UE 项目骨架（Blueprint-only，无需编译 C++）。
你需要做的只有装引擎、装插件、填两个 key。

## 1. 装 Unreal Engine 5.5（约 30 分钟，需要 Epic 账号）

```bash
brew install --cask epic-games
```

打开 **Epic Games Launcher** → 登录 → Unreal Engine 标签页 → Library →
`+` 安装 **UE 5.5.x**（Apple Silicon 原生，本机 M1 Pro/32GB 可流畅跑编辑器）。

## 2. 装 Cesium for Unreal 插件（免费）

Epic Launcher → **Fab**（原 Marketplace）搜 **"Cesium for Unreal"** →
Add to Library → 在 Library 里对 UE 5.5 点 **Install to Engine**。

## 3. 申请两个 key（都免费起步）

1. **Cesium ion token**：https://ion.cesium.com 注册 → Access Tokens → 复制默认 token
2. **Google Photorealistic 3D Tiles key**：https://console.cloud.google.com →
   启用 **Map Tiles API** → 创建 API key
   （若暂时不想接 Google，可跳过——用 Cesium World Terrain + OSM Buildings 白模城市也能先跑通全部玩法）

## 4. 打开项目

双击 `IslandGrid.uproject`（首次打开会编译 shader，等几分钟）。

首次进入后按 `DESIGN.md` 第 1 节搭 `Maps/SingaporeTwin` 关卡：
Cesium 面板 → 登录 ion → 一键添加 Google Photorealistic 3D Tiles →
把 CesiumGeoreference 的 Origin 设为 `lat 1.34, lng 103.82`。

## 5. 导入数据表

Content Browser → `Data/` → 右键 Import → 选 `Content/Data/Sites.csv`、`Faults.csv`
（先按 `DESIGN.md` 第 3 节建对应的 Struct：`FSiteRow`、`FFaultRow`）。

## 6. 后续开发

按 `DESIGN.md` 的 Blueprint 清单和四周路线图推进。
遇到具体实现问题（Niagara 弧线、Cesium 多边形高亮、Level Sequence 编排）随时找 Claude —
设计文档里每个组件都写明了用哪个引擎特性实现。
