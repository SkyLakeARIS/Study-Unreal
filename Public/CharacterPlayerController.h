// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Version.h"
#include "GameFramework/PlayerController.h"
#include "CharacterPlayerController.generated.h"


class ASRPlayerState;
class UUISelectModesWidget;
class UUIResultWidget;
class UUIHUDWidget;
class UUIPauseWidget;
/**
 * 
 */
UCLASS()
class VERSION_API ACharacterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACharacterPlayerController();
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* InPawn) override;
	void ChangeInputMode(bool bIsGameMode);
	
	void ResumeCountDown();
	void ClearCountDown();

	void BindStatToUI();

	void ShowInGameHUDAndStartTimer();

	void SetDebugMode(bool active);
	bool IsDebugging() const;
	ASRPlayerState* GetPlayerState() const;
	UUIHUDWidget* GetIngameHUD() const;
	UUISelectModesWidget* GetSelectModesWidget() const;
	class ASRTargetManager* GetTargetManager() const;

protected:

	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void PauseGame();

	void countDownMainTime();

	void countReadyTime();

	void result();


protected:

	UPROPERTY()
	TSubclassOf<UUIPauseWidget> mPauseWidgetClass;
		
	UPROPERTY()
	TSubclassOf<UUserWidget> HUDClass;

	UPROPERTY()
	TSubclassOf<UUIResultWidget> mReusltWidgetClass;

	UPROPERTY()
	TSubclassOf<UUISelectModesWidget> mSelectModesWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ASRTargetManager* mTargetManager;

private:

	UPROPERTY()
	UUIHUDWidget* InGameHUD;
	UPROPERTY()
	UUIPauseWidget* mPauseWidget;	// esc 키 UI (current tap key)
	UPROPERTY()
	UUIResultWidget* mResultWidget;
	UPROPERTY()
	UUISelectModesWidget* mSelectModesWidget;

	UPROPERTY()
	ASRPlayerState* mPlayerState;

	bool mbDebugMode;

	FTimerHandle THCountDown;
	uint32 remainingTime;
	uint32 mTimeToReady;

	FInputModeGameOnly GameInputMode;
	FInputModeUIOnly UIInputMode;

	float mSensitivity;
};
