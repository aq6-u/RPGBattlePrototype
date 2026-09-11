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

---

## Day 2（2026.09.11）

### 今日完成

开始进入 C++ Editor Plugin / Quick Asset Actions 阶段，创建独立 Editor 类型插件，并完成第一个可运行的 Asset Action：

- 创建 Blank 插件，并配置 .uplugin，将插件类型从 Runtime 调整为 Editor，同时修改 LoadingPhase 为 PreDefault。
- 创建继承自 AssetActionUtility 的 C++ 类，配置 Blutility 依赖及相关 Private Include Path。
- 使用 UFUNCTION(CallInEditor) 暴露编辑器函数，完成第一个 Quick Asset Action，并成功在 UE 编辑器的 Scripted Asset Actions 中调用。
- 封装公共调试打印函数，将屏幕输出与 UE_LOG 统一封装，减少重复调试代码。
- 学习并实践批量复制资产，使用 UEditorUtilityLibrary::GetSelectedAssetData() 获取当前选中的资产，并通过 UEditorAssetLibrary::DuplicateAsset() 与 SaveAsset() 完成批量复制。
- 理解 UE 资产路径中 AssetName、ObjectPath 与 PackagePath 的区别，并明确其在资产操作中的不同用途。

### 今日思考

今天正式从 Editor Utility 蓝图进入 C++ Editor Tool 开发，开始接触 UE 编辑器扩展的底层实现方式。

相比 Day 1 中通过 EditorUtilityWidget 快速搭建原型，C++ Editor Plugin 的扩展能力更强，也更适合后续实现 DataTable 批量处理、Content Browser 自定义操作等功能。

今天最大的收获并不是完成“批量复制资产”这一具体功能，而是开始建立对 UE 编辑器工具工作方式的整体认识：通过插件提供编辑器功能，再通过 AssetActionUtility 将自定义操作接入编辑器资产工作流。

后续将继续学习 Content Browser 自定义菜单、Delegate、Editor Tab、Slate 与 SListView，并逐步将这些能力应用到 DataTable 数据校验工具中。

> **开发耗时**：约 3 h