// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SRPlayerState.h"
#include "Version.h"
#include "GameFramework/PlayerController.h"
#include "CharacterPlayerController.generated.h"

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
	// 위젯 클래스 구현 필요(menu, HUD, result)
	// esc 키 바인드 액션 
public:
	ACharacterPlayerController();
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* InPawn) override;
	void ChangeInputMode(bool bIsGameMode);
	
	void ResumeCountDown();
	void ClearCountDown();

	void BindStatToUI();


	ASRPlayerState* GetPlayerState() const;
	UUIHUDWidget* GetIngameHUD() const;
protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
private:
	UFUNCTION()
	void PauseGame();

	void countDown();
	
	void result();
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<UUIPauseWidget> mPauseWidgetClass;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=UI)
	TSubclassOf<UUserWidget> HUDClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<UUIResultWidget> mReusltWidgetClass;
private:
	
	UPROPERTY()
	UUIHUDWidget* InGameHUD;

	UPROPERTY()
	UUIPauseWidget* mPauseWidget;	// esc 키 UI (current tap key)
	
	UPROPERTY()
	UUIResultWidget* mResultWidget;
	
	UPROPERTY()
	ASRPlayerState* mPlayerState;
	
	FTimerHandle THCountDown;
	uint32 remainingTime;
	
	FInputModeGameOnly GameInputMode;
	FInputModeUIOnly UIInputMode;
};
