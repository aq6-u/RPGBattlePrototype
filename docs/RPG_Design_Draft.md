# 基于 GAS 与行为树的近战动作 RPG 战斗原型 (RPGBattlePrototype)

> **项目状态**：`draft_v2.0` 设计草案
> **实现方案**：数据驱动设计 / 事件解耦管线 / 状态机与标签矩阵控制

**说明**：本草案为项目启动前设计规划，部分内容为预期目标，实际开发中根据可用素材与工期进行了调整，详见 DevLog 与项目总结文档。


## 1. 项目简介与研发目标

本项目使用虚幻引擎 5（UE5）完成一个 1v1 近战首领战（Boss战）的核心玩法原型。研发过程中遵循 MVP（最小可行性产品）开发原则，不包含背包、关卡、任务等外围系统，集中于动作博弈、蓝图/C++ 混合架构以及数据配置管线的实现。

### 技术要点

1. **数据与表现解耦**：采用全局事件订阅子系统，隔离伤害结算与 UI 刷新逻辑。
2. **帧级精准格挡（Parry）**：利用 `AnimNotifyState` 在动画帧内建立严格的时间窗防御判定，结合 GameplayTag 矩阵实现受击反馈。
3. **数据驱动配置**：基于虚幻原生 `PrimaryDataAsset` 组织首领招式与数值配置，方便后续扩展。


## 2. MVP（最小可行性产品）取舍原则

为了在 20 天内完成核心内容的开发并保证功能闭环，项目进行了明确的功能剪裁：

### 2.1 功能保留项

- **战斧两连击 Combo**：仅保留 `Primary_Attack_A` 与 `B`，简化动画状态机与输入缓存逻辑。
- **帧级 Parry（精准格挡）**：实现判定时间窗、顿帧（Time Dilation）以及 Boss 脱力僵直状态锁。
- **DataAsset 数据驱动**：使用 `PrimaryDataAsset` 存储单 Boss 的招式数值、冷却和蒙太奇指针。
- **Leap（飞扑）招式**：二阶段核心招式。采用"锁定玩家实时坐标 → 飞扑位移 → 触发落地 AOE"的直击方案。
- **行为树 AI 与二阶段流转**：保留一阶段双足慢跑、二阶段四足狂奔、锁血咆哮转场的行为树管线。

### 2.2 功能剪裁项

- **战斧三连击**：去掉第三段劈砍，减少连续输入容错与多段判定框偏移的处理成本。
- **普通格挡与耐力值系统**：不做普通挡格减伤及耐力（Stamina）扣减机制，博弈保留"精准格挡，翻滚闪避，受击倒地"。
- **EQS（环境查询系统）**：不用行为树中的 EQS 网格射线扫描，避免复杂的边界碰撞调试。
- **杂兵与升级系统**：去掉刷怪点、属性刷新及升级 UI，开局直接切入 Boss 战。


## 3. 核心 Gameplay 循环架构

整个战斗循环围绕"施压 → 应对 → 释放 → 阶段流转"的逻辑构建：

玩家进入战局后，空气墙与 UI 血条激活。战斗开始后，若玩家长时间保持远距离，Boss 会释放远程投石施加压迫；若玩家切入近战，Boss 释放近战大砸地。此时玩家面临决策：在砸地生效前 6 帧内按下右键触发精准格挡（Parry），成功后触发全局顿帧与 Boss 脱力僵直，随后进入绝对释放窗口进行连击输出；若失败或未应对，玩家受击倒地硬直 1.2 秒。当 Boss 生命值低于 50% 时，触发二阶段转场——锁血咆哮、移速提升 70%、解锁飞扑大招。


## 4. 玩家系统设计

### 4.1 类设计与职责边界：`BP_PlayerCharacter`

- **核心职责**：承载物理胶囊体碰撞、基础移动组件（CharacterMovement）；接收增强输入映射；驱动动画状态机并挂载 `UActorComponent_Combat`（战斗组件）管理生命值。
- **边界红线**：不负责 UI 的绘制与刷新，血条和顿帧视觉效果通过事件异步广播，由 UI 监听器独立处理。

### 4.2 增强输入与移动

- `IA_Move` (Axis2D) → W/A/S/D
- `IA_LightAttack` (Digital) → 鼠标左键
- `IA_Parry` (Digital) → 鼠标右键
- `IA_Dash` (Digital) → 左 Shift 键

基础移动参数：
- `MaxWalkSpeed` = 550.0f
- `BrakingDecelerationWalking` = 2000.0f

### 4.3 战斧两连击 Combo 系统

将连击精简为 A → B 两连击，利用 `AnimNotifyState` 在动画帧内管理输入缓存。

玩家按下左键时，先检查当前是否含有 `State.Attacking` 标签。若处于攻击状态，则进一步判断是否在 `ComboWindow` 内，若是则设置缓存布尔值 `bSaveCombo = True`，若否则拒绝缓存输入。若不处于攻击状态，则播放 `Primary_Attack_A` 蒙太奇并赋予 `State.Attacking` 标签。

`Primary_Attack_A`（全长 45 帧）动画内部配置如下：
- 第 15 - 35 帧：配置 `AnimNotifyState_ComboWindow`，在此区间内按下左键将 `bSaveCombo` 设为 True。
- 第 20 帧：配置 `AnimNotify_ApplyDamage`，调用 `GetSocketLocation` 触发球体武器射线检测。
- 第 36 帧：配置 `AnimNotify_ComboCheck`，若 `bSaveCombo == True` 则调用 `PlayMontage(Primary_Attack_B)` 并将 `bSaveCombo` 复位；若为 False，动作自然播放至结束，在动画结束通知中清除 `State.Attacking` 标签。

### 4.4 帧级 Parry（精准格挡）实现

当玩家按下右键（`IA_Parry`）时，系统驱动角色播放举盾动画蒙太奇：

- 第 1 - 6 帧：配置 `AnimNotifyState`，动态赋予玩家 `State.ParryWindow` 标签。若在此 6 帧内重叠了 Boss 攻击判定盒，则判定精准格挡成功。
- 第 7 帧起：该标签被自动剥离。若在此之后重叠了 Boss 攻击判定盒，则判定防御失败，执行普通受击反应。
- 边缘情况：若在前 6 帧内玩家松开了右键，系统通过 `OnInputReleased` 强行中断动画，清除 `State.ParryWindow`，防止通过高频重复输入刷新 Parry 窗口。

### 4.5 受击分级与生命值

- 生命值：`MaxHP` = 100.0f，`CurrentHP` 初始为 100.0f。
- 受击判定按 GameplayTag 区分：携带 `Tag.Boss.Skill.Light` 时不打断当前动作，仅扣血并显示飘字；携带 `Tag.Boss.Skill.Heavy` 且玩家未处于 Parry/Dash 状态时，强制停止所有蒙太奇，播放倒地动画 1.2 秒，期间赋予 `State.Disabled` 标签屏蔽输入。


## 5. 首领 AI 系统设计

### 5.1 类设计：`BP_BossCharacter`

**核心职责**：
- 维护 Boss 属性（`MaxHP` = 300.0f）。
- 承载骨骼模型（缩放 2.6 倍）及双手 Socket 挂点。
- 挂载 AI 控制器 `AIC_BossController`，在 BeginPlay 时运行行为树。

**不负责**：感知逻辑硬编码，距离刷新和状态切换由行为树任务节点完成。

### 5.2 行为树与黑板设计

**黑板变量**：
- `SelfActor` (Object) → Boss 自身
- `TargetActor` (Object) → 玩家
- `DistanceToTarget` (Float) → 实时玩家间距
- `bIsEnraged` (Bool) → 二阶段暴走标志（默认 False）
- `bIsStunned` (Bool) → 脱力僵直状态锁（默认 False）

**行为树结构**：
- 根节点为 Selector，按优先级判断：
  1. 若 `bIsStunned == True`，挂起所有攻击，播放 Recover 动画。
  2. 若 `bIsEnraged == True`，进入二阶段选择器：距离 > 600 时执行 LeapAttack，距离 < 400 时执行 EnragedPound。
  3. 默认走一阶段选择器：距离 > 600 时执行 RockThrow，距离 < 400 时执行 GroundPound。

### 5.3 核心技能逻辑实现

**技能 A：碎裂裂地砸（GroundPound）**

触发时机为一阶段 `DistanceToTarget <= 400.0f`。行为树执行 `BTTask_PlayMontage(Ability_L_GroundPound)`。在蒙太奇第 33 帧（双拳砸地瞬间）通过 `AnimNotify_SpawnOverlapBox` 生成半径 350 码的圆形伤害区，重叠此区域且没有 `State.ParryWindow` / `State.Dashing` 标签的 Actor 受到 30 点伤害并附带 `Tag.Boss.Skill.Heavy`（触发倒地）。第 34 帧通过 `AnimNotify_SetStunLock` 将 `bIsStunned` 设为 True，强制播放 1.5 秒恢复动画，期间 Boss 挂载 `Tag.Boss.State.Vulnerable`（受击增伤 30%）。

**技能 B：远古巨石投掷（RockThrow）**

触发时机为一阶段 `DistanceToTarget > 600.0f`。前 20 帧播放拔石动画，第 15 帧在右手 Socket 生成 `BP_ProjectileRock` 岩石子弹；第 21-45 帧过渡至投掷动画，第 30 帧通过 `AnimNotify_ReleaseRock` 激活岩石的 `ProjectileMovement` 组件朝玩家当前位置飞行。岩石命中玩家时携带 `Tag.Boss.Skill.Heavy`，触发重度倒地。

**技能 C：飞扑（Leap Attack）**

触发时机为二阶段 `DistanceToTarget > 600.0f`。行为树执行 `BTTask_LeapAttack`，获取玩家瞬时坐标写入 `MoveToLocation`。Boss 播放起跳蒙太奇并关闭与玩家的碰撞，使用 `VInterpTo` 将胶囊体平滑位移至目标坐标，落地播放着陆蒙太奇并恢复碰撞，触发 500 码 AOE 伤害，携带 `Tag.Boss.Skill.Heavy`。

### 5.4 二阶段转场机制

当 Boss HP 首次低于 50%（150 HP）时，属性组件派发 `Event_OnHealthThreshold`。系统立即中断当前行为树任务，播放咆哮蒙太奇并锁血 2 秒，设置黑板 `bIsEnraged = True`，`MaxWalkSpeed` 改为 600.0f，切换四足奔跑动画。


## 6. 数据驱动与标签字典

单 Boss 原型使用 `PrimaryDataAsset` 进行可视化配置，不用外部表格。

### 6.1 `PDA_BossSkillData` 数据资产结构

- `SkillTag` (FGameplayTag)：技能唯一标识
- `BaseDamage` (float)：该招式基础伤害
- `Cooldown` (float)：招式内置 CD
- `AnimationMontage` (UAnimMontage*)：绑定的动画蒙太奇

### 6.2 核心 GameplayTag 字典

- `State.Attacking`：攻击中，互斥其他动作输入
- `State.ParryWindow`：精准格挡有效窗口
- `State.Dashing`：闪避中，物理碰撞临时关闭
- `State.Disabled`：倒地/强直，屏蔽控制权
- `Tag.Boss.Skill.Heavy`：Boss 重攻击，命中后打断玩家并触发倒地
- `Tag.Boss.State.Vulnerable`：Boss 僵直期间易伤标记（受击增伤 30%）


## 7. 模块解耦：伤害结算管线

为了避免 `BP_PlayerCharacter` 与 `BP_BossCharacter` 之间产生双向引用，伤害判定与 UI 刷新流程通过全局事件订阅机制完成。

Boss 伤害判定盒产生重叠后，发送伤害事件（含技能 Tag 与基础伤害）至战斗管理器子系统。子系统检索玩家当前状态 Tag 字典进行仲裁：若玩家持有 `State.ParryWindow`，则判定 Parry 成功，触发顿帧并广播 Parry 成功事件至 UI 层；若玩家未持有任何无敌 Tag，则扣除全额生命值并触发重度受击倒地状态锁，广播生命值变动信号，UI 层刷新血条百分比并触发受击飘字。


## 8. 仓库目录结构

```
Content
├── _ProjectCore
│   ├── Architecture    # 核心解耦类、Combat组件、全局事件管理器
│   ├── Input           # 增强输入映射
│   └── System          # DataAsset定义、GameplayTag字典
├── Characters
│   ├── Player_Terra    # 玩家资产（动画、材质、控制蓝图）
│   └── Boss_Rampage    # Boss资产（动画、AI行为树、黑板、BTTasks）
└── UI
    └── WBP_HUD_Main    # 血条与伤害飘字组件
```


## 9. 20天工程排期表

| 天数   | 模块               | 进度控制点                                               |
| :----- | :----------------- | :------------------------------------------------------- |
| Day 1  | 项目初始化         | 新建项目，导入资产，建立文件夹结构                       |
| Day 2  | 玩家基础移动       | 配置增强输入，连通八向跑步与移动组件                     |
| Day 3  | 普攻第一击         | 串连 `Primary_Attack_A` 蒙太奇，实现球体射线伤害检测     |
| Day 4  | 攻击 Combo 链      | 编写 `AnimNotifyState_ComboWindow`，实现 A→B 输入缓存    |
| Day 5  | 闪避无敌帧         | 连通闪避动画，通过通知管理无敌帧与碰撞开关               |
| Day 6  | **验证节点一**     | 测试两连击 Combo + 闪避手感                              |
| Day 7  | 战斗组件与属性系统 | 创建 `UActorComponent_Combat`，建立 C++ 属性集           |
| Day 8  | Parry 状态判定     | 实现右键盾档，配置前 6 帧 `State.ParryWindow` 赋予与移除 |
| Day 9  | Parry 效果闭环     | 实现顿帧、格挡特效、触发 Boss 僵直                       |
| Day 10 | 受击矩阵联动       | 导入受击倒地蒙太奇，连通重攻击强制打断与倒地逻辑         |
| Day 11 | **验证节点二**     | 调试 Parry 判定窗与受击倒地反馈                          |
| Day 12 | AI 行为树奠基      | 建立行为树与黑板，配置 Boss 体型与一阶段追击 Task        |
| Day 13 | AI 近战砸地        | 编写 GroundPound 任务，连通红圈判定与僵直锁              |
| Day 14 | AI 远程投石        | 编写 RockThrow 任务，配置大于 600 码自动触发             |
| Day 15 | 转场与二阶段       | 实现血量阈值转场、锁血咆哮、移速提升                     |
| Day 16 | 飞扑招式           | 编写 LeapAttack 任务，平滑位移至玩家坐标并落地 AOE       |
| Day 17 | **验证节点三**     | 跑通完整一阶段拉扯 + 二阶段飞扑 AI 循环                  |
| Day 18 | 数据资产重构       | 将伤害、冷却、蒙太奇迁移至 `PrimaryDataAsset`            |
| Day 19 | 核心 HUD           | 接入血条 UI，微调顿帧时间                                |
| Day 20 | **打包交付**       | 冻结代码，打包 Standalone，录制实机演示视频              |


## 10. 设计依据（樱井政博理论映射）

| 设计决策                             | 对应理论                   |
| :----------------------------------- | :------------------------- |
| 一阶段慢速练习 Parry，二阶段加速压迫 | 有峰有谷的难度曲线         |
| Parry 成功 → Boss 僵直 → 全力输出    | 压力的施加与释放           |
| Parry 窗口严苛，失败即倒地           | 风险回报理论               |
| 远程巨石逼迫玩家进入近战             | 敌人存在的意义——施压       |
| 50% 血量强制转阶段                   | 电脑角色设计（转阶段）     |
| 飞扑预判玩家位移                     | 为游戏增色的随机性（适度） |
| 僵直期提供绝对输出窗口（易伤标记）   | 优先考虑奖励元素           |
