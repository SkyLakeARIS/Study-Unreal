// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Version.h"
#include "Blueprint/UserWidget.h"
#include "UIResultWidget.generated.h"

class UTextBlock;
class UButton;

/*
 *  게임이 종료되면 표시되는 결과 UI 클래스입니다.
 *	플레이어가 획득한 점수, 전적에 대한 정보를 표시합니다.
 */
UCLASS()
class VERSION_API UUIResultWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION()
	void UpdateStageInfo(class ASRPlayerState* srPlayerState);
		
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
	UTextBlock* mScore;
	UPROPERTY()
	UTextBlock* mAccuracy;
	UPROPERTY()
	UTextBlock* mKill;
};
