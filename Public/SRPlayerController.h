// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "GameModeData.h"
#include "Version.h"
#include "GameFramework/PlayerController.h"
#include "SRPlayerController.generated.h"

class ASRTargetManager;
class ASRPlayerState;
class UUISelectModesWidget;
class UUIResultWidget;
class UUIHUDWidget;
class UUIPauseWidget;

/*
 * 플레이어 캐릭터 컨트롤러 클래스입니다.
 */
UCLASS()
class VERSION_API ASRPlayerController final : public APlayerController
{
	GENERATED_BODY()

public:
	ASRPlayerController();

	void StartGame();
	void EndGame();

	UFUNCTION()
	void PauseGame();
	void ResumeGame();

	void ChangeInputMode(const bool bIsGameMode);
	void CreateUIWidgets();

	void UpdateTargetPositionFrom(const ASRTargetManager& targetManager);
	void CalcTargetIndicatorAndShow();

	/*
	 *  set data to character
	 */
	void SetMouseSensitivityAndUpdateToCharacter(const FMouseSensitivity newSetting);
	void SetAimingTypeToCharacter(const eAimingType newSetting);
	void InitCharacterMouseAndAimingSetting(const eScopeType scopeType) const;

	/*
	 *  getter
	 */
	ASRPlayerState* GetPlayerState() const;
	UUIHUDWidget* GetHUDWidget() const;
	UUISelectModesWidget* GetSelectWidget() const;
	FMouseSensitivity GetMouseSensitivity() const;

	// save/load data
	void LoadMouseSensitivitySetting();
	void SaveMouseSensitivitySetting() const;

protected:

	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

	void initTargetIndicator();
protected:
	/*
	 *  ui
	 */
	UPROPERTY()
	TSubclassOf<UUIPauseWidget> mPauseWidgetClass;
	UPROPERTY()
	UUIPauseWidget* mPauseWidget;

	UPROPERTY()
	TSubclassOf<UUIResultWidget> mReusltWidgetClass;
	UPROPERTY()
	UUIResultWidget* mResultWidget;

	UPROPERTY()
	TSubclassOf<UUISelectModesWidget> mSelectModesWidgetClass;
	UPROPERTY()
	UUISelectModesWidget* mSelectModesWidget;

	UPROPERTY()
	TSubclassOf<UUserWidget> mHUDClass;
	UPROPERTY()
	UUIHUDWidget* mHUDWidget;

	UPROPERTY()
	TSubclassOf<UUserWidget> mTargetIndicateLine1440Class;		// 화면 크기에 따라서 수평선 위젯을 다르게 불러오기 위함. (비율 문제로 인해서.)
	UPROPERTY()
	TSubclassOf<UUserWidget> mTargetIndicateLine1080Class;
	UPROPERTY()
	TSubclassOf<UUserWidget> mTargetIndicateLine900Class;
	UPROPERTY()
	TSubclassOf<UUserWidget> mTargetIndicateLine720Class;
	UPROPERTY()
	TSubclassOf<UUserWidget> mTargetIndicateLine480Class;
	UPROPERTY()
	TSubclassOf<UUserWidget> mTargetIndicateLineOtherSizeClass;
	UPROPERTY()
	UUserWidget* mTargetIndicateLineWidget;						// 수평선 위젯

	UPROPERTY()
	TSubclassOf<UUserWidget> mTargetIndicatorClass;
	UPROPERTY()
	UUserWidget* mTargetIndicatorWidget[2];						// 수평선 위젯 위에서 방향을 표시해 줄 위젯


private:

	TArray<FVector, TFixedAllocator<8>> mSpawnedTargetList;

	int32 mViewportX;
	int32 mViewportY;
	float mIndicatorScale;
	/*
	 *  class
	 */
	UPROPERTY()
	ASRPlayerState* mPlayerState;

	/*
	 *  input mode data
	 */
	FInputModeGameOnly GameInputMode;
	FInputModeUIOnly UIInputMode;

	/*
	 *  input data
	 */
	FMouseSensitivity mMouseSetting;
	eAimingType mAimingType;
};