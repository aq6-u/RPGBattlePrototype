# RPGBattlePrototype V2.0 重构开发日志

> 项目：UE5 战斗原型（RPGBattlePrototype）
> 阶段：V2.0 架构重构


## Day 1（2026.09.05）

### 今日完成

#### 1. T3：Ability 与动画解耦——Tag 清理重构

- 将 Tag 清理逻辑从 Montage Ended 迁移至 Ability 生命周期回调
- 在 Play Montage and Wait 的 On Cancelled / On Interrupted 引脚后接入 Remove Loose Gameplay Tag
- 覆盖范围：E、Q、AirMelee、Melee、Boss的七个技能
- 经过多轮验证，结果通过

#### 2. Dash / Jump 状态互斥修复

- 发现 Dash 视觉结束后 Tag 仍存在，连续 Dash + Jump 会导致速度异常累积
- 临时方案：将 Jump 纳入 GA 管理，Dash Tag 存在时禁止 Jump

#### 3. T6：UI 数据绑定迁移到 C++

- 取消 UMG 中 ProgressBar 的蓝图 Create Binding 轮询方式
- 改为 C++ 层通过 GetGameplayAttributeValueChangeDelegate 监听属性变化
- 通过 BlueprintImplementableEvent 将更新事件抛回蓝图侧，由蓝图执行 UI 刷新
- 覆盖范围：Player 的 HP、MP、Strength，Boss 的 HP

#### 4. 技能 Cost 校验修复

- 修复 E、Q、Dash 技能在 MP/Strength 不足时仍能释放的问题
- 在 GA 的 EventActivateAbility 开头增加 Cost 校验逻辑

#### 5. 修复 Player 重生后血条显示为空的问题

- 原因：BeginPlay 中绑定属性变化委托后未主动推送初始值，导致 UI 默认显示 0
- 解决方案：在 BeginPlay 绑定委托后，主动调用 OnHPUpdated / OnMPUpdated / OnStrengthUpdated 推送当前属性值
- 仅推送 Player 自身属性，BossHP 不受影响

### 今日思考

今天排查的几个问题分布在不同的模块里，但看下来它们都在指向同一个方向——状态的更新和管理。

T3 做下来有一个明显的感觉：Tag 的生命周期不应该跟着动画事件走。Montage Ended 只能表示动画播放完成，但 Ability 可能因为打断、取消或其他原因结束，这时候动画事件根本不会触发，Tag 就留在身上了。把清理逻辑放到 OnCanceled / OnInterrupted 之后，不管 Ability 以什么方式退出，Tag 都能被正常移除。

T6 在 V1.0 Day 2 的时候尝试过，当时没有跑通，所以先用 UMG 的 Create Binding 作为临时方案。UMG 的 Create Binding 本质上是蓝图层每帧轮询属性值，在编辑器里跑没什么问题，但数据驱动的方式更自然一些——属性变化时主动通知 UI，而不是 UI 反复去问属性有没有变。改成 Delegate 之后，C++ 这边负责监听和分发事件，蓝图只做具体的更新操作，边界也更清楚。

完成T6后发现Player重生后血条显示为空的问题。虽然BeginPlay 里绑定了 Delegate，但绑定本身不会触发一次回调把当前值推过去，所以我让其在BeginPlay里主动推送更新属性。

Cost 校验也是类似的情况——GA 正常能激活，但激活时没有检查资源是否足够。功能实现了，但入口处没有把异常输入挡住。

V1.0 的阶段目标是让功能跑起来，V2.0 更多是在补那些“跑起来之后才会暴露”的边界情况。状态是否在任何结束路径下都能正确清理、UI 是否在正确时机拿到正确的初始值、非法输入是否在入口就被拦截——这些都是功能本身之外的细节，但堆在一起决定了系统是否可靠。

> **开发耗时**：约 5 h
