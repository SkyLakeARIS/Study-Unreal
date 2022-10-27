// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "GameModeData.h"
#include "Version.h"
#include "GameFramework/PlayerController.h"
#include "SRPlayerController.generated.h"

class ASRPlayerState;
class UUISelectModesWidget;
class UUIResultWidget;
class UUIHUDWidget;
class UUIPauseWidget;

/*
 * 플레이어 캐릭터 컨트롤러 클래스입니다.
 */
UCLASS()
class VERSION_API ASRPlayerController : public APlayerController
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

	/*
	 *  set data to character
	 */
	void SetMouseSensitivityAndUpdateToCharacter(const FMouseSensitivity newSetting);
	void SetAimingTypeToCharacter(const EAimingType newSetting);
	void InitCharacterMouseAndAimingSetting(const EScopeType scopeType) const;

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

private:
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
	EAimingType mAimingType;
};