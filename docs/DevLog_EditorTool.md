# Editor Tool DevLog

> UE5 Editor Tool 开发学习与实践记录
> 以 DataTable 数据校验工具为实践项目，跟随 C++ Editor Tool 开发教程，学习 UE 编辑器扩展、AssetActionUtility、Content Browser 菜单、Delegate、Slate、SListView 等相关开发流程，并最终完成一个可实际运行的编辑器工具。

## Day 1（2026.09.08）

### 今日完成

创建 EditorUtilityWidget 蓝图作为 DataTable 校验工具的原型。在 UMG 设计器中为 Player 和 Boss 的 DataTable 分别创建按钮，点击后执行对应的校验逻辑：

- Player 方向：硬编码指定 DT_PlayerSkill，Break 结构体 ST_PlayerSkill，对 Damage、Cooldown、Cost、GE_Damage 等字段做校验
- Boss 方向：硬编码指定 DT_BossSkill，Break 结构体 ST_BossSkill，对 Damage、Cooldown、Cost、GE_Damage 等字段做校验
- 异常时通过 format 结合 Print String 打印错误信息（含行名 + 异常原因）
- 两个按钮分别调用对应的校验逻辑，校验均已跑通

### 今日思考

今天实际写了一些逻辑后，发现蓝图开始变得臃肿。尝试将不同角色的校验分别封装成函数，虽然主事件图表干净了一些，但函数内部依然有大量 Branch + Format Text + Print 的组合——每个字段都要写一套判断逻辑，封装后函数本身还是比较臃肿。

也尝试过使用 EditorUtilityWidget 中的 Get Selected Assets，但目前无法做到在 Content Browser 中选中 DataTable 后直接进行校验。如果要在 Widget 中让用户自由选择 DT，也没有找到可以直接拖拽使用的资产选择控件，导致目前的交互方式不太顺畅。

考虑到后续还需要继续完善这个工具，包括更多字段、多表支持和修改功能，继续在蓝图上堆功能可能会让结构越来越复杂。因此暂时停在这个原型阶段，之后直接开始跟随 C++ Editor Tool 开发教程学习。

> **开发耗时**：约 2.5 h