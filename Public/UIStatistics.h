// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "Blueprint/UserWidget.h"
#include "UIStatistics.generated.h"

class USRStatistics;
class UTextBlock;
/**
 * 
 */
UCLASS()
class VERSION_API UUIStatistics final : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void UpdateStatistics();

private:

	// ar stats
	UPROPERTY()
	UTextBlock* mARFireshots;
	UPROPERTY()
	UTextBlock* mARHits;
	UPROPERTY()
	UTextBlock* mARAccuracy;
	UPROPERTY()
	UTextBlock* mARHeadshots;
	UPROPERTY()
	UTextBlock* mARHeadshotRate;
	UPROPERTY()
	UTextBlock* mARKills;
	UPROPERTY()
	UTextBlock* mARSelected;

	// hg stats
	UPROPERTY()
	UTextBlock* mHGFireshots;
	UPROPERTY()
	UTextBlock* mHGHits;
	UPROPERTY()
	UTextBlock* mHGAccuracy;
	UPROPERTY()
	UTextBlock* mHGHeadshots;
	UPROPERTY()
	UTextBlock* mHGHeadshotRate;
	UPROPERTY()
	UTextBlock* mHGKills;
	UPROPERTY()
	UTextBlock* mHGSelected;

	// sr stats
	UPROPERTY()
	UTextBlock* mSRFireshots;
	UPROPERTY()
	UTextBlock* mSRHits;
	UPROPERTY()
	UTextBlock* mSRAccuracy;
	UPROPERTY()
	UTextBlock* mSRHeadshots;
	UPROPERTY()
	UTextBlock* mSRHeadshotRate;
	UPROPERTY()
	UTextBlock* mSRKills;
	UPROPERTY()
	UTextBlock* mSRSelected;

	// game stats
	UTextBlock* mSelectedBattlefield;
	UPROPERTY()
	UTextBlock* mSelectedRainbowSix;
	UPROPERTY()
	UTextBlock* mSelectedEscapeFromTarkov;

	// mode score stats
	UPROPERTY()
	UTextBlock* mStaticShortRangeScore;
	UPROPERTY()
	UTextBlock* mStaticMidRangeScore;
	UPROPERTY()
	UTextBlock* mStaticLongRangeScore;
	UPROPERTY()
	UTextBlock* mMovableShortRangeScore;
	UPROPERTY()
	UTextBlock* mMovableMidRangeScore;
	UPROPERTY()
	UTextBlock* mMovableLongRangeScore;
};
