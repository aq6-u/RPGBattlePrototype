#include "CustomStyle/SuperManagerStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/StyleColors.h"


FName FSuperManagerStyle::StyleSetName = FName("SuperManagerStyle");
TSharedPtr<FSlateStyleSet> FSuperManagerStyle::CreatedSlateStyleSet = nullptr;

void FSuperManagerStyle::InitializeIcons()
{
    if (!CreatedSlateStyleSet.IsValid())
    {
        CreatedSlateStyleSet = CreateSlateStyleSet();
        FSlateStyleRegistry::RegisterSlateStyle(*CreatedSlateStyleSet);
    }
}

TSharedRef<FSlateStyleSet> FSuperManagerStyle::CreateSlateStyleSet()
{
    TSharedRef<FSlateStyleSet> CustomStyleSet = MakeShareable(new FSlateStyleSet(StyleSetName));

    const FString IconDirectory = IPluginManager::Get().FindPlugin(TEXT("SuperManager"))->GetBaseDir() / "Resources";

    CustomStyleSet->SetContentRoot(IconDirectory);

    const FVector2D Icon16x16 (16.f, 16.f);
    CustomStyleSet->Set("ContentBrowser.DeleteUnusedAssets", new FSlateImageBrush(IconDirectory / "Icon128.png", Icon16x16));

    CustomStyleSet->Set("ContentBrowser.DeleteEmptyFolders",
        new FSlateImageBrush(IconDirectory / "Icon128.png", Icon16x16));

    CustomStyleSet->Set("ContentBrowser.AdvanceDeletion",
        new FSlateImageBrush(IconDirectory / "Icon128.png", Icon16x16));

    CustomStyleSet->Set("LevelEditor.LockSelection",
        new FSlateImageBrush(IconDirectory / "Icon128.png", Icon16x16));

    CustomStyleSet->Set("LevelEditor.UnlockSelection",
        new FSlateImageBrush(IconDirectory / "Icon128.png", Icon16x16));

    const FCheckBoxStyle SelectionLockToggleButtonStyle = FCheckBoxStyle()
        .SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
        .SetPadding(FMargin(10.f))

        .SetUncheckedImage(FSlateImageBrush(IconDirectory / "Icon128.png", Icon16x16, FStyleColors::White25))
        .SetUncheckedHoveredImage(FSlateImageBrush(IconDirectory / "Icon128.png", Icon16x16, FStyleColors::AccentBlue))
        .SetUncheckedPressedImage(FSlateImageBrush(IconDirectory / "Icon128.png", Icon16x16, FStyleColors::Foreground))

        .SetCheckedImage(FSlateImageBrush(IconDirectory / "Icon128.png", Icon16x16, FStyleColors::Foreground))
        .SetCheckedHoveredImage(FSlateImageBrush(IconDirectory / "Icon128.png", Icon16x16, FStyleColors::AccentBlue))
        .SetCheckedPressedImage(FSlateImageBrush(IconDirectory / "Icon128.png", Icon16x16, FStyleColors::AccentBlue));

    CustomStyleSet->Set("SceneOutliner.SelectionLock", SelectionLockToggleButtonStyle);

    return CustomStyleSet;
}

void FSuperManagerStyle::ShutDown()
{
    if (CreatedSlateStyleSet.IsValid())
    {
        FSlateStyleRegistry::UnRegisterSlateStyle(*CreatedSlateStyleSet);
        CreatedSlateStyleSet.Reset();
    }
}