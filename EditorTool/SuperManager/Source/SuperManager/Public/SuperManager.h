// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FSuperManagerModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

#pragma region ContentBrowserMenuExtention

	void InitCBMenuExtention();

	TArray<FString> FolderPathsSelected;
	TSharedRef<FExtender> CustomCBMenuExtender(const TArray<FString>& SelectedPaths);

	void AddCBMenuEntry(class FMenuBuilder& MenuBuilder);

	void OnDeleteUnusedAssetButtonClicked();

	void FixUpRedirectors();

	void OnDeleteEmptyFoldersButtonClicked();

	void OnAdvanceDeletionButtonClicked();
#pragma endregion

#pragma region CustomEditorTab

	void RegisterAdvanceDeletionTab();

	TSharedRef<SDockTab> OnSpawnAdvanceDeltionTab(const FSpawnTabArgs& SpawnTabArgs);
	TArray<TSharedPtr<FAssetData>>GetAllAssetDataUnderSelectedFolder();
	TSharedPtr<SDockTab> ConstructedDockTab;

	void OnAdvanceDeletionTabClosed(TSharedRef<SDockTab> TabToClose);

#pragma endregion

#pragma region LevelEditorMenuExtension

	void InitLevelEditorExtention();

	TSharedRef<FExtender> CustomLevelEditorMenuExtender(const TSharedRef<FUICommandList> UICommandList, const TArray<AActor*> SelectedActors);

	void AddLevelEditorMenuEntry(class FMenuBuilder& MenuBuilder);

	void OnLockActorSelectionButtonClicked();

	void OnUnlockActorSelectionButtonClicked();
#pragma endregion

#pragma region SelectionLock

	void InitCustomSelectionEvent();

	void OnActorSelected(UObject* SelectedObject);

	void LockActorSelection(AActor* ActorToProcess);
	void UnlockActorSelection(AActor* ActorToProcess);

	void RefreshSceneOutliner();
#pragma endregion

	TWeakObjectPtr<class UEditorActorSubsystem> WeakEditorActorSubsystem;

	bool GetEditorActorSubsystem();

#pragma region CustomEditorUICommands

	TSharedPtr<class FUICommandList> CustomUICommands;

	void InitCustomUICommands();

	void OnSelectionLockHotKeyPressed();

	void OnUnlockActorSelectionHotKeyPressed();

#pragma endregion

#pragma region SceneOutlinerExtension

	void InitSceneOutlinerColumnExtension();

	TSharedRef<class ISceneOutlinerColumn> OnCreateSelectionLockColumn(class ISceneOutliner& SceneOutliner);

	void UnRegisterSceneOutlinerColumnExtension();

#pragma endregion

public:
#pragma region ProcessDataForAdvanceDeletionTab

	bool DeleteSingleAssetForAssetList(const FAssetData& AssetDataToDelete);
	bool DeleteMultipleAssetsForAssetList(const TArray<FAssetData>& AssetsToDelete);
	void ListUnusedAssetsForAssetList(const TArray< TSharedPtr < FAssetData> >& AssetDataToFilter, TArray< TSharedPtr < FAssetData> >& OutUnusedAssetsData);
	void ListSameNameAssetsForAssetList(const TArray< TSharedPtr < FAssetData> >& AssetDataToFilter, TArray< TSharedPtr < FAssetData> >& OutSameNameAssetsData);
	void SyncCBToClickedAssetForAssetList(const FString& AssetPathToSync);
#pragma endregion

	bool CheckIsActorSelectionLocked(AActor* ActorToProcess);
	void ProcessLockingForOutliner(AActor* ActorToProcess, bool bShouldLock);

};
