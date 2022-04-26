// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Version.h"
#include "Blueprint/UserWidget.h"
#include "UIHUDWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class VERSION_API UUIHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION()
	void UpdateRemainingTime(int32 remainingTime);
	UFUNCTION()
	void UpdateAmmo(int32 newAmmo);
	UFUNCTION()
	void UpdateAccuracy(int32 accuracy);
	//UFUNCTION()
	//void UpdateKillCount();	// HUD에서는 킬카운트 x
	UFUNCTION()
	void UpdateFireMode(EWaeponFireMode newFireMode);
	UFUNCTION()
	void UpdateScore(int32 score);
	UFUNCTION()
	void UpdateGameMode(FString newGameMode);
protected:

	virtual void NativeConstruct() override;

private:

	
	UPROPERTY()	// UPROPERTY( meta =(BindWidget))�� ����ϸ� NativeConstruct()���� ���� ���ε� �����൵��. �ٸ� �������� �������Ʈ�� �̸��� ���ƾ���.
	UTextBlock* RemainingTime;
	
	UPROPERTY()
	UTextBlock* Accuracy;
	
	UPROPERTY()
	UTextBlock* FireMode;
	
	UPROPERTY()
	UTextBlock* RemainingAmmo;
	//
	//UPROPERTY()
	//UTextBlock* KillCount;
	
	UPROPERTY()
	UTextBlock* Score;

	//UPROPERTY(meta = (BindWidget))
	UTextBlock* DisplayGameMode;
};
