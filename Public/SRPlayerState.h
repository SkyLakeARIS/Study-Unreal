// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Version.h"
#include "GameFramework/PlayerState.h"
#include "SRPlayerState.generated.h"

class UUIHUDWidget;
DECLARE_DELEGATE_OneParam(FOnUpdateScore, int32)
DECLARE_DELEGATE_OneParam(FOnUpdateAccuray, int32)

/*
 * �� ���� ���� �÷��̾��� ����� �����ϴ� ���� Ŭ�����Դϴ�.
 * �� Ŭ������ �������� HUD�� ǥ�õ˴ϴ�.
 */
UCLASS()
class VERSION_API ASRPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASRPlayerState();
	UFUNCTION()
	void BindHUD(UUIHUDWidget* HUD);

	UFUNCTION()
	void OnAddScore(int32 getScore);
	UFUNCTION()
	void OnAddFireShots(int32 shots);
	UFUNCTION()
	void OnHitCount();	// hit������ delegate ����
	UFUNCTION()
	void OnKill();		// kill������ delegate ����
	int32 GetScore() const;
	int32 GetAccuracy() const;
	int32 GetKillCount() const;
private:
	void calcAccuracy();
private:
	FOnUpdateScore mOnUpdateScore;
	FOnUpdateAccuray mOnUpdateAccuracy;
	int32 mHits;
	int32 mFireShots;	// result ui���� ���
	int32 mAccuracy;	// hud, result ui���� ���
	int32 mScore;	// hud, result ui���� ���
	int32 mKill;	// result ui���� ���.
};
