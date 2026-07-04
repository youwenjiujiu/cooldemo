# Island Grid · Unreal 5 数字孪生 — 设计与实现文档

目标：**可交互、有游戏感**的新加坡能源数字孪生。用户可以自由飞行、点击任意资产楼、
触发三条故事线（Energy Efficiency / Energy Management / IoC+FDD）。
照片级真实城市，发布会与客户演示两用。

---

## 1. 世界搭建（Cesium）

| 组件 | 配置 |
|---|---|
| `CesiumGeoreference` | Origin: lat `1.3400`, lng `103.8200`, height `250m` |
| `Cesium3DTileset` (主城市) | **Google Photorealistic 3D Tiles** — Cesium ion Asset ID `2275207`（用 ion 转发）或直连 Google API（Tileset Source = From URL, `https://tile.googleapis.com/v1/3dtiles/root.json?key=GOOGLE_KEY`） |
| 备选（免 Google key） | Cesium World Terrain (ion `1`) + Cesium OSM Buildings (ion `96188`)，白模城市，配夜景材质也很好看 |
| `CesiumSunSky` | 时间锁定黄昏 19:15（暖橙天光 + 城市灯光最出效果）|
| PostProcessVolume | Bloom intensity ~1.2，Lumen GI，轻微 vignette，色温偏冷 |

> Google 3D Tiles 有免费额度（每月 root.json 请求限量），演示场景够用；
> 生产部署前在 Google Cloud Console 给 key 加 referrer/应用限制。

## 2. 交互模型（游戏感的来源）

**两种模式，随时切换：**

- **Story Mode（开场默认）**：Level Sequence 驱动的三幕电影导览（可 Skip）。
  每幕结束停在可交互状态，提示 "click any asset"。
- **Explore Mode**：`CesiumFlyToPawn`/DynamicPawn 自由飞行（WASD + 鼠标，滚轮调速），
  点击任意资产信标 → 相机 fly-to 环绕 + 弹出资产卡片。

**点击命中**：每个资产是一个 `BP_AssetBeacon`（带 SphereCollision），
鼠标 LineTrace 命中信标即选中；命中 Google tileset 本体时，取最近 500m 内的信标。

## 3. 核心 Blueprint 清单

| Blueprint | 职责 |
|---|---|
| `BP_TwinGameMode` | 模式切换（Story/Explore）、故事线状态机 |
| `BP_AssetBeacon` | 资产信标：CesiumGlobeAnchor 定位（读 Sites DataTable 的经纬度）；Niagara 光柱 + 脉冲环；WidgetComponent 名牌；Hero/Mock 两档大小；点击事件 |
| `BP_AssetCard` (UMG) | 点击资产后的卡片：org 名、当前故事状态、Sensing/Predicting/Acting 三行 + 收敛曲线（UMG Image + 材质动画，复刻网页版 tuning card） |
| `BP_FaultDirector` | 读 Faults DataTable，按时间触发：信标转红脉冲 → FDD 卡片四行推进（Detected/Diagnosed/Dispatched/Resolved）→ 转绿 |
| `BP_EnergyArc` | Niagara Ribbon：资产间网状能量流（最近邻 mesh，同网页版拓扑，蓝紫→青渐变粒子流） |
| `BP_StoryDirector` | 三个 Level Sequence 的编排 + chapter 字幕（左上角）|
| `WBP_HUD` | 模式切换按钮、章节指示点、Skip/Replay |

## 4. 三条故事线 → UE 实现

### 第一幕 · Energy Efficiency（樟宜）
- Level Sequence：相机从全岛俯瞰 fly-to 樟宜航站楼群（Google tiles 里真实的 T1-T4 + Jewel）
- 航站楼高亮：`Cesium Polygon Raster Overlay` 圈住航站楼范围调亮/着色（Google tiles 不能按楼选择，用多边形叠加高亮区域）
- 楼上弹出 tuning 卡片（WidgetComponent），Sensing→Predicting→Acting + 收敛曲线动画

### 第二幕 · Energy Management（组合到全岛）
- 相机拉高，35 个 BP_AssetBeacon 依次点亮（Hero 先、Mock 快速横扫）
- BP_EnergyArc 织网（Niagara ribbon 沿贝塞尔弧线，粒子沿弧流动）

### 第三幕 · IoC + FDD
- 相机全岛慢巡，BP_FaultDirector 触发三个故障事件
- 红色脉冲 + FDD 卡片生命周期，解决后恢复
- 结束进入 Explore Mode，用户自己点

## 5. 分发

| 途径 | 说明 |
|---|---|
| macOS/Windows 桌面包 | 首选。Shipping 打包 ~1-2GB，现场演示/发客户 |
| Pixel Streaming | 网页访问、无需安装；需 GPU 服务器（AWS g4dn/g5 或自建），UE 内置插件，Phase 2 再上 |

## 6. 路线图（假设 1 名 UE 开发全职）

| 周 | 里程碑 |
|---|---|
| 1 | 环境 + Cesium + Google tiles 出图；自由飞行；35 信标从 DataTable 落地 |
| 2 | 三个 Level Sequence 电影导览；航站楼多边形高亮；tuning/FDD 卡片 |
| 3 | 能量网 Niagara；点击交互 + 资产卡；HUD/模式切换；打包 |
| 4 | 打磨（光照/音效/过场）、性能（tiles 流式预算）、Pixel Streaming 评估 |

## 7. 与网页版的关系

网页版（repo 根目录 `index.html`）继续作为**发链接就能看**的轻量版；
UE 版是**现场演示/展厅**的重武器。两者共用同一份站点与故事数据
（`Content/Data/Sites.csv`、`Faults.csv` 与网页版 `SITES`/`FAULTS` 保持同步）。
