# SuperManager - UE5 C++ Editor Tool

这是一个基于 UE5 C++ 开发的编辑器扩展插件，旨在优化游戏开发中的资产管理、场景搭建与编辑器工作流。

## 核心功能
- **Content Browser 扩展**：右键菜单新增“删除未使用资产”、“删除空文件夹”等一键清理功能，并在执行前自动修复重定向器（Redirector）。
- **Slate 高级删除面板**：基于 SListView 构建独立 Dock Tab，支持按“全部/未使用/同名”筛选资产，支持一键同步定位 Content Browser 位置，并包含非空文件夹保护机制以降低误删风险。
- **场景 Actor 批处理**：支持一键选中场景中所有同名 Actor，支持按指定轴向、数量、间距批量复制 Actor。
- **Actor 变换随机化**：支持对选中 Actor 的位移、旋转（Yaw/Pitch/Roll）、缩放进行批量随机化。
- **Actor 选择锁定**：通过 Level Editor 右键菜单或自定义快捷键（Alt+W / Alt+Shift+W），锁定特定 Actor 防止误选，并在 Scene Outliner 中新增自定义列（CheckBox）以直接控制锁定状态。
- **自定义 UI**：使用 FSlateStyleSet 注册自定义图标，并应用于菜单与面板。

## 安装与使用
1. 将整个 `SuperManager` 文件夹复制到你的 UE5 项目的 `Plugins` 目录下。
2. 右键点击项目 `.uproject` 文件，选择 `Generate Visual Studio project files`。
3. 使用 Visual Studio 编译项目（Development Editor / Win64）。
4. 打开 UE5 编辑器，在 `Edit -> Plugins` 中启用 `SuperManager` 并重启。
5. 在 Content Browser 文件夹右键菜单，或 Level Editor 的 Actor 右键菜单中即可使用对应功能。

## 演示视频
[B站实机演示（P2为编辑器工具演示）](https://www.bilibili.com/video/BV1knYx68E5M)