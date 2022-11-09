// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "Blueprint/UserWidget.h"
#include "UIResultWidget.generated.h"

class ASRPlayerState;
class UTextBlock;
class UButton;

/*
 *  ������ ����Ǹ� ǥ�õǴ� ��� UI Ŭ�����Դϴ�.
 *	�÷��̾ ȹ���� ����, ������ ���� ������ ǥ���մϴ�.
 */
UCLASS()
class VERSION_API UUIResultWidget final : public UUserWidget
{
	GENERATED_BODY()
public:

	void UpdateStageInfo(const ASRPlayerState& srPlayerState);
		
protected:

	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void onRetryClicked();
	UFUNCTION()
	void onStageClicked();
	UFUNCTION()
	void onMenuClicked();

public:
	
	UPROPERTY()
	UButton* mBtnRetry;
	UPROPERTY()
	UButton* mBtnStage;
	UPROPERTY()
	UButton* mBtnMenu;

	UPROPERTY()
	UTextBlock* mScores;
	UPROPERTY()
	UTextBlock* mAccuracy;
	UPROPERTY()
	UTextBlock* mKills;
	UPROPERTY()
	UTextBlock* mShots;
	UPROPERTY()
	UTextBlock* mHeadshotRate;

};
