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

---

## Day 3（2026.09.13）

### 今日完成

继续学习 Quick Asset Actions，完成消息提示封装、资产前缀自动添加、未使用资产删除与重定向器修复等功能：

- 将 ShowMsgDialog（模态弹窗）与 ShowNotifyInfo（右下角通知）统一封装到 DebugHeader.h，用于替代之前分散在 .cpp 中的临时提示逻辑，并根据操作结果区分两种提示方式。
- 使用 TMap<UClass*, FString> 建立资产类型与命名前缀的映射关系，遍历选中的资产，根据资产类型自动匹配前缀并调用 RenameAsset 完成批量重命名。
- 处理材质实例的特殊命名规则：重命名前先移除原有的 M_ 前缀和 _Inst 后缀，再添加 MI_ 前缀，避免出现前缀重复或命名不规范的问题。
- 使用 UEditorAssetLibrary::FindPackageReferencersForAsset 检查资产引用情况，将没有引用的资产加入待删除列表，再通过 ObjectTools::DeleteAssets 进行批量删除。
- 使用 Asset Registry 收集 /Game 路径下的 UObjectRedirector，通过 FAssetToolsModule::FixupReferencers 批量修复重定向器引用，并在删除未使用资产前先完成重定向器修复。
- 根据新增功能补充 Build.cs 模块依赖，加入 UnrealEd 与 AssetTools，解决 ObjectTools、重定向器修复等相关接口的编译依赖问题。

### 今日思考

今天继续学习 Quick Asset Actions，开始接触到一些比“实现功能”更偏向编辑器工具实际使用体验和资产工作流的问题。

例如消息提示、批量重命名、未使用资产清理这些功能，本身实现起来并不算特别复杂，但真正做成编辑器工具后，需要考虑不同操作的使用场景，以及资产之间的引用关系。尤其是删除资产和修复重定向器的部分，让我进一步认识到 UE 编辑器中的资产并不是简单的文件，修改和删除操作都需要考虑 Asset Registry、引用关系以及 Package 等底层机制。

材质实例的命名处理也是一个比较典型的细节。不同类型资产可能有各自约定俗成的命名规则，因此批量处理不能简单地对所有资产套用同一套字符串逻辑，而需要根据资产类型做针对性处理。

经过这几天的学习，Quick Asset Actions 部分基本完成。相比 Day 1 的 EditorUtilityWidget 原型，现在已经开始能够通过 C++ 插件直接介入 UE 的资产工作流。接下来将进入 Content Browser 自定义菜单的学习，让 DataTable 校验工具能够从“独立窗口中的按钮”进一步转变为“选中 DataTable 后直接通过右键菜单执行”，逐步向真正可用的编辑器工具过渡。

> **开发耗时**：约 4 h

---

## Day 4（2026.09.15）

### 今日完成

继续完善 Editor Tool 的 Content Browser 快捷菜单功能，实现无用资产搜索、Redirector 修复以及空文件夹清理。

- **Search And Delete Unused Assets**
  - 获取 Content Browser 当前选中的文件夹路径
  - 使用 UEditorAssetLibrary::ListAssets 递归搜索文件夹下的资产
  - 排除 Developers、Collections、ExternalActors**、**ExternalObjects 等特殊目录
  - 通过 FindPackageReferencersForAsset 查询资产引用关系，筛选无引用资产
  - 使用 ObjectTools::DeleteAssets 批量删除确认后的无用资产
- **Fix Up Redirectors**
  - 使用 AssetRegistry 搜索 /Game 下的 UObjectRedirector
  - 将搜索结果转换为 UObjectRedirector 对象
  - 通过 AssetTools::FixupReferencers 修复 Redirector 引用，并处理 Redirector 删除
- **Delete Empty Folders**
  - 获取选中文件夹下的目录结构
  - 排除特殊目录，并检查目录是否存在
  - 修正原教程中 ture 等明显代码错误
  - 针对原逻辑可能误删非空文件夹的问题，重新设计空文件夹判断逻辑
  - 只有确认目录下不存在资产及子文件夹时，才执行 DeleteDirectory
  - 删除前再次进行检查，降低递归删除导致误删的风险

### 今日思考

今天开始接触资产清理相关功能后，发现 Editor Tool 与普通游戏逻辑相比，对操作安全性的要求更高。

尤其是在实现空文件夹清理时，原教程中的判断逻辑存在一定风险：“没有资产”并不等于“文件夹为空”。如果目录中仍然存在子文件夹，而后续调用的是递归删除接口，就可能将原本不应该删除的内容一并删除。

因此，在保留教程整体实现思路的基础上，对这部分逻辑进行了调整，将“是否存在资产”和“是否真正为空”区分开，并在执行删除前再次进行检查。

同时也进一步熟悉了 AssetRegistry、AssetTools、UEditorAssetLibrary 等 Editor API 在资产管理场景中的配合方式。

> **开发耗时**：约 3 h

---

## Day 5（2026.09.16）

### 今日完成

开始学习 Editor Tool 的 Slate Widget 开发流程，开始将前面实现的 Content Browser 功能与自定义 Slate 界面连接起来。

* 学习 UE5 Slate 中智能指针的基本使用方式，了解 TSharedPtr、TSharedRef、TWeakPtr 与 UObject GC 体系之间的区别
* 创建 Advance Deletion 编辑器 Tab，并通过 FGlobalTabmanager 注册和唤起自定义 Nomad Tab
* 创建 SAdvanceDeletionTab，继承 SCompoundWidget 构建自定义 Slate 界面
* 使用 SVerticalBox、SHorizontalBox、STextBlock、SScrollBox 等 Slate 控件搭建基础界面
* 使用 SListView 展示选中文件夹下的资产，并通过 STableRow 自定义列表项
* 将前面获取的资产数据转换为 TSharedPtr<FAssetData>，传递给 Slate Widget 使用
* 补充 Slate、SlateCore 等模块依赖，并完成基础编译与界面搭建

### 今日思考

今天开始真正接触 Slate 后，对 UE Editor Tool 的界面构建方式有了进一步认识。

之前通过 EditorUtilityWidget 可以比较直观地完成工具原型，但当工具需要更复杂的编辑器界面和资产列表时，Slate 提供了更加灵活的实现方式。今天的 Advance Deletion 主要还是跟随教程完成基础结构，但已经开始理解 Editor Tab → Slate Widget → Asset Data → SListView 之间的数据和界面关系。

同时也进一步认识到 UE 中智能指针与 UObject GC 并不是同一套内存管理机制。Slate 中大量使用 TSharedPtr / TSharedRef 管理非 UObject 类型对象，而编辑器资产本身仍然属于 UE 的 UObject 体系，需要根据对象类型选择合适的指针方式。

目前完成的 Slate 界面仍属于基础版本，后续将继续学习 Slate Widget、Delegate、SListView 等内容，并逐步完善 Advance Deletion 工具的实际交互功能。

> **开发耗时**：约 3.5 h

---

## Day 6（2026.09.17）

### 今日完成

继续学习 Editor Tool 的 Slate Widget 开发，进一步完善 Advance Deletion 编辑器工具，将前面获取到的资产数据接入 Slate 列表界面，并补充列表项的交互逻辑。

- 将 Content Browser 中获取到的 FAssetData 数据保存到 StoredAssetsData，作为 SListView 的数据源
- 使用 SListView 展示选中文件夹下的资产，并通过 STableRow 自定义每一行的显示内容
- 为列表项添加 CheckBox，使工具能够记录用户选择的资产
- 实现资产删除相关的交互逻辑，将 Slate 界面中的选择结果与前面的资产数据处理流程连接起来
- 在资产操作完成后刷新列表，使界面状态与实际资产内容保持同步
- 进一步熟悉 Slate 中数据 → ListView → TableRow → 控件交互的基本开发流程

### 今日思考

今天的学习让我对 Slate 的开发方式有了更具体的认识。

之前主要是在学习如何创建 Editor Tab、构建基础 Slate 界面，以及如何获取 Content Browser 中的资产数据。到了今天，开始真正把这些部分串联起来：先获取 FAssetData，再保存为列表数据源，通过 SListView 创建列表，最后由 STableRow 负责具体的列表项显示和交互。

相比 EditorUtilityWidget，Slate 的开发过程更加偏向“数据驱动 UI”。界面本身并不直接保存业务数据，而是通过 ItemsSource 等方式读取数据，再由列表和行控件生成对应的视觉内容。这种方式虽然学习成本更高，但也让我开始理解 UE 编辑器工具中数据、界面和交互之间如何进行解耦。

目前 Advance Deletion 已经从最初的简单功能原型，逐渐发展成一个具有资产获取、列表展示、选择、删除和刷新流程的完整 Editor Tool 结构。后续将继续学习 Delegate、SListView 等相关内容，并进一步完善工具的交互功能。

> **开发耗时**：约 4 h