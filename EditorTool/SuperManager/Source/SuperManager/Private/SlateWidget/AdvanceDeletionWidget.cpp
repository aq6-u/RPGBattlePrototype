#include "SlateWidget/AdvanceDeletionWidget.h"
#include "SlateBasics.h"
#include "DebugHeader.h"
#include "SuperManager.h"

#define ListAll TEXT("List All Available Assets")
#define ListUnused TEXT("List Unused Assets")
#define ListSameName TEXT("List Assets With Same Name")

void SAdvanceDeletionTab::Construct(const FArguments& InArgs)
{
    bCanSupportFocus = true;

    StoredAssetsData = InArgs._AssetsDataToStore;
    DisplayedAssetsData = StoredAssetsData;
    CheckBoxesArray.Empty();
    AssetsDataToDeleteArray.Empty();
    ComboBoxSourceItems.Empty();

    ComboBoxSourceItems.Add(MakeShared<FString>(ListAll));
    ComboBoxSourceItems.Add(MakeShared<FString>(ListUnused));
    ComboBoxSourceItems.Add(MakeShared<FString>(ListSameName));

    FSlateFontInfo TitleTextFont = GetEmboseedTextFont();
    TitleTextFont.Size = 30;

    ChildSlot
        [
            SNew(SVerticalBox)

            + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Advance Deletion")))
                        .Font(TitleTextFont)
                        .Justification(ETextJustify::Center)
                        .ColorAndOpacity(FColor::White)
                ]

            + SVerticalBox::Slot()
                .AutoHeight()
                [

                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            ConstructComboBox()
                        ]

                        + SHorizontalBox::Slot()
                        .FillWidth(.6f)
                        [
                            ConstructComboHelpTexts(TEXT("Specify the listing condition in the drop down. Left mouse click to go to where asset is located"), ETextJustify::Center)
                        ]

                        + SHorizontalBox::Slot()
                        .FillWidth(.1f)
                        [
                            ConstructComboHelpTexts(TEXT("Current Folder:\n") + InArgs._CurrentSelectedFolder, ETextJustify::Right)
                        ]
                ]

            + SVerticalBox::Slot()
                .VAlign(VAlign_Fill)
                [
                    SNew(SScrollBox)

                        + SScrollBox::Slot()
                        [
                            ConstructAssetListView()
                        ]
                ]

            + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SHorizontalBox)

                        + SHorizontalBox::Slot()
                        .FillWidth(10.f)
                        .Padding(5.f)
                        [
                            ConstructDeleteAllButton()
                        ]

                        + SHorizontalBox::Slot()
                        .FillWidth(10.f)
                        .Padding(5.f)
                        [
                            ConstructSelectAllButton()
                        ]

                        + SHorizontalBox::Slot()
                        .FillWidth(10.f)
                        .Padding(5.f)
                        [
                            ConstructDeselectAllButton()
                        ]
                ]

        ];
}

TSharedRef<SListView<TSharedPtr<FAssetData>>> SAdvanceDeletionTab::ConstructAssetListView()
{
    ConstructedAssetListView = SNew(SListView< TSharedPtr <FAssetData> >)
        .ItemHeight(24.f)
        .ListItemsSource(&DisplayedAssetsData)
        .OnGenerateRow(this, &SAdvanceDeletionTab::OnGenerateRowForList)
        .OnMouseButtonClick(this, &SAdvanceDeletionTab::OnRowWidgetMouseButtonClicked);

    return ConstructedAssetListView.ToSharedRef();
}

void SAdvanceDeletionTab::RefreshAssetListView()
{
    CheckBoxesArray.Empty();
    AssetsDataToDeleteArray.Empty();
    if (ConstructedAssetListView.IsValid())
    {
        ConstructedAssetListView->RebuildList();
    }
}

#pragma region ComboBoxForListingCondition

TSharedRef<SComboBox<TSharedPtr<FString>>> SAdvanceDeletionTab::ConstructComboBox()
{
    TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructedComboBox =
        SNew(SComboBox<TSharedPtr<FString>>)
        .OptionsSource(&ComboBoxSourceItems)
        .OnGenerateWidget(this, &SAdvanceDeletionTab::OnGenerateComboContent)
        .OnSelectionChanged(this, &SAdvanceDeletionTab::OnComboSelectionChanged)
        [
            SAssignNew(ComboDisplayTextBlock, STextBlock)
                .Text(FText::FromString(TEXT("List Assets Option")))
        ];

    return ConstructedComboBox;
}

TSharedRef<SWidget> SAdvanceDeletionTab::OnGenerateComboContent(TSharedPtr<FString> SourceItem)
{
    TSharedRef<STextBlock> ConstructedComboText = SNew(STextBlock)
        .Text(FText::FromString(*SourceItem.Get()));

    return ConstructedComboText;
}

void SAdvanceDeletionTab::OnComboSelectionChanged(TSharedPtr<FString> SelectedOption,
    ESelectInfo::Type InSelectInfo)
{
    DebugHeader::Print(*SelectedOption.Get(), FColor::Cyan);
    ComboDisplayTextBlock->SetText(FText::FromString(*SelectedOption.Get()));

    FSuperManagerModule& SuperManagerModule =
        FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));

    if (*SelectedOption.Get() == ListAll)
    {
        DisplayedAssetsData = StoredAssetsData;
        RefreshAssetListView();
    }
    else if (*SelectedOption.Get() == ListUnused)
    {
        SuperManagerModule.ListUnusedAssetsForAssetList(StoredAssetsData, DisplayedAssetsData);
        RefreshAssetListView();
    }
    else if (*SelectedOption.Get() == ListSameName)
    {
        SuperManagerModule.ListSameNameAssetsForAssetList(StoredAssetsData, DisplayedAssetsData);
        RefreshAssetListView();
    }
}

TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructComboHelpTexts(const FString& TextContent, ETextJustify::Type TextJustify)
{
    TSharedRef<STextBlock> ConstructedHelpText =
        SNew(STextBlock)
        .Text(FText::FromString(TextContent))
        .Justification(TextJustify)
        .AutoWrapText(true);
    return ConstructedHelpText;
}

#pragma endregion

#pragma region RowWidgetForAssetListView

TSharedRef<ITableRow> SAdvanceDeletionTab::OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
    if (!AssetDataToDisplay.IsValid()) return SNew(STableRow < TSharedPtr <FAssetData> >, OwnerTable);

    const FString DisplayAssetClassName = AssetDataToDisplay->AssetClassPath.GetAssetName().ToString();
    const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();

    FSlateFontInfo AssetClassNameFont = GetEmboseedTextFont();
    AssetClassNameFont.Size = 10;

    FSlateFontInfo AssetNameFont = GetEmboseedTextFont();
    AssetNameFont.Size = 15;

    TSharedRef < STableRow < TSharedPtr <FAssetData> > > ListViewRowWidget = SNew(STableRow < TSharedPtr <FAssetData> >, OwnerTable).Padding(FMargin(5.f))
        [
            SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .HAlign(HAlign_Left)
                .VAlign(VAlign_Center)
                .FillWidth(.05f)
                [
                    ConstructCheckBox(AssetDataToDisplay)
                ]

                + SHorizontalBox::Slot()
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Fill)
                .FillWidth(.5f)
                [
                    ConstructTextForRowWidget(DisplayAssetClassName, AssetClassNameFont)
                ]

                + SHorizontalBox::Slot()
                .HAlign(HAlign_Left)
                .VAlign(VAlign_Fill)
                [
                    ConstructTextForRowWidget(DisplayAssetName, AssetNameFont)
                ]

                + SHorizontalBox::Slot()
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Fill)
                [
                    ConstructButtonForRowWidget(AssetDataToDisplay)
                ]
        ];

    return ListViewRowWidget;
}

void SAdvanceDeletionTab::OnRowWidgetMouseButtonClicked(TSharedPtr<FAssetData> ClickedData)
{
    FSuperManagerModule& SuperManagerModule =
        FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));

    SuperManagerModule.SyncCBToClickedAssetForAssetList(ClickedData->ObjectPath.ToString());
}

TSharedRef<SCheckBox> SAdvanceDeletionTab::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
    TSharedRef<SCheckBox> ConstructedCheckBox = SNew(SCheckBox)
        .Type(ESlateCheckBoxType::CheckBox)
        .OnCheckStateChanged(this, &SAdvanceDeletionTab::OnCheckBoxStateChanged, AssetDataToDisplay)
        .Visibility(EVisibility::Visible);
    CheckBoxesArray.Add(ConstructedCheckBox);
    return ConstructedCheckBox;
}

void SAdvanceDeletionTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
    switch (NewState)
    {
    case ECheckBoxState::Unchecked:
        if (AssetsDataToDeleteArray.Contains(AssetData))
        {
            AssetsDataToDeleteArray.Remove(AssetData);
        }
        break;
    case ECheckBoxState::Checked:
        AssetsDataToDeleteArray.AddUnique(AssetData);
        break;
    case ECheckBoxState::Undetermined:
        break;
    default:
        break;
    }
}

TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo& FontToUse)
{
    TSharedRef<STextBlock> ConstructedTextBlock = SNew(STextBlock)
        .Text(FText::FromString(TextContent))
        .Font(FontToUse)
        .ColorAndOpacity(FColor::White);
    return ConstructedTextBlock;
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructButtonForRowWidget(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
    TSharedRef<SButton> ConstructedButton = SNew(SButton)
        .Text(FText::FromString(TEXT("Delete")))
        .OnClicked(this, &SAdvanceDeletionTab::OnDeleteButtonClicked, AssetDataToDisplay);
    return ConstructedButton;
}

FReply SAdvanceDeletionTab::OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData)
{
    FSuperManagerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));

    const bool bAssetDeleted = SuperManagerModule.DeleteSingleAssetForAssetList(*ClickedAssetData.Get());

    if (bAssetDeleted)
    {
        if (StoredAssetsData.Contains(ClickedAssetData))
        {
            StoredAssetsData.Remove(ClickedAssetData);
        }

        if (DisplayedAssetsData.Contains(ClickedAssetData))
        {
            DisplayedAssetsData.Remove(ClickedAssetData);
        }

        RefreshAssetListView();
    }

    return FReply::Handled();
}

#pragma endregion

#pragma region TabButtons

TSharedRef<SButton> SAdvanceDeletionTab::ConstructDeleteAllButton()
{
    TSharedRef<SButton> DeleteAllButton = SNew(SButton)
        .ContentPadding(FMargin(5.f))
        .OnClicked(this, &SAdvanceDeletionTab::OnDeleteAllButtonClicked);

    DeleteAllButton->SetContent(ConstructTextForTabButtons(TEXT("Delete All")));
    return DeleteAllButton;
}

FReply SAdvanceDeletionTab::OnDeleteAllButtonClicked()
{
    if (AssetsDataToDeleteArray.Num() == 0)
    {
        DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No asset currently selected"));
        return FReply::Handled();
    }

    TArray<FAssetData> AssetDataToDelete;
    for (const TSharedPtr<FAssetData>& Data : AssetsDataToDeleteArray)
    {
        AssetDataToDelete.Add(*Data.Get());
    }

    FSuperManagerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));

    const bool bAssetsDeleted = SuperManagerModule.DeleteMultipleAssetsForAssetList(AssetDataToDelete);

    if (bAssetsDeleted)
    {
        for (const TSharedPtr<FAssetData>& DeletedData : AssetsDataToDeleteArray)
        {
            if (StoredAssetsData.Contains(DeletedData))
            {
                StoredAssetsData.Remove(DeletedData);
            }
            if (DisplayedAssetsData.Contains(DeletedData))
            {
                DisplayedAssetsData.Remove(DeletedData);
            }
        }
        RefreshAssetListView();
    }
    return FReply::Handled();
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructSelectAllButton()
{
    TSharedRef<SButton> SelectAllButton = SNew(SButton)
        .ContentPadding(FMargin(5.f))
        .OnClicked(this, &SAdvanceDeletionTab::OnSelectedAllButtonClicked);

    SelectAllButton->SetContent(ConstructTextForTabButtons(TEXT("Select All")));
    return SelectAllButton;
}

FReply SAdvanceDeletionTab::OnSelectedAllButtonClicked()
{
    if (CheckBoxesArray.Num() == 0) return FReply::Handled();

    for (const TSharedRef<SCheckBox>& CheckBox : CheckBoxesArray)
    {
        if (!CheckBox->IsChecked())
        {
            CheckBox->ToggleCheckedState();
        }
    }

    return FReply::Handled();
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructDeselectAllButton()
{
    TSharedRef<SButton> DeselectAllButton = SNew(SButton)
        .ContentPadding(FMargin(5.f))
        .OnClicked(this, &SAdvanceDeletionTab::OnDeselectAllButtonClicked);

    DeselectAllButton->SetContent(ConstructTextForTabButtons(TEXT("Deselect All")));
    return DeselectAllButton;
}

FReply SAdvanceDeletionTab::OnDeselectAllButtonClicked()
{
    if (CheckBoxesArray.Num() == 0) return FReply::Handled();

    for (const TSharedRef<SCheckBox>& CheckBox : CheckBoxesArray)
    {
        if (CheckBox->IsChecked())
        {
            CheckBox->ToggleCheckedState();
        }
    }

    return FReply::Handled();
}

TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructTextForTabButtons(const FString& TextContent)
{
    FSlateFontInfo ButtonTextFont = GetEmboseedTextFont();
    ButtonTextFont.Size = 15;

    TSharedRef<STextBlock> ConstructedTextBlock = SNew(STextBlock)
        .Text(FText::FromString(TextContent))
        .Font(ButtonTextFont)
        .Justification(ETextJustify::Center);

    return ConstructedTextBlock;
}

#pragma endregion