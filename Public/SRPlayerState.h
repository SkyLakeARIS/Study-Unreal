// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SRPlayerState.generated.h"

class UUIHUDWidget;
DECLARE_DELEGATE_OneParam(FOnUpdateScore, int32)
DECLARE_DELEGATE_OneParam(FOnUpdateAccuray, int32)
//DECLARE_DELEGATE(FOnUpdateKillCount);
/**
 * 
 */
UCLASS()
class VERSION_API ASRPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASRPlayerState();
	void BindHUD(UUIHUDWidget* HUD);

	UFUNCTION()
	void OnAddScore(int32 getScore);
	UFUNCTION()
	void OnAddFireShots(int32 shots);
	UFUNCTION()
	void OnHitCount();	// hit판정시 delegate 실행
	UFUNCTION()
	void OnKill();		// kill판정시 delegate 실행
	int32 GetScore();
	int32 GetAccuracy();
	int32 GetKillCount();
private:
	void calcAccuracy();
private:
	FOnUpdateScore mOnUpdateScore;
	FOnUpdateAccuray mOnUpdateAccuracy;
//	FOnUpdateKillCount mOnupdateKillCount;
	int32 mHits;
	int32 mFireShots;	// result ui에서 사용
	int32 mAccuracy;	// hud, result ui에서 사용
	int32 mScore;	// hud, result ui에서 사용
	int32 mKill;	// result ui에서 사용.
};
