// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "GameFramework/PlayerState.h"
#include "SRPlayerState.generated.h"

class USRStatistics;
enum class EGameType : uint8;
enum class EWeaponType : uint8;
enum class EGameModeType : uint8;
class UUIHUDWidget;
DECLARE_DELEGATE_OneParam(FOnUpdateScore, int32)
DECLARE_DELEGATE_OneParam(FOnUpdateAccuray, int32)

/*
 * 한 게임 동안 플레이어의 기록을 저장하는 전적 클래스입니다.
 * 이 클래스의 정보들이 HUD에 표시됩니다.
 */
UCLASS()
class VERSION_API ASRPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	ASRPlayerState();

	void initialize(EWeaponType weapon, EGameType game, EGameModeType mode);

	UFUNCTION()
	void BindHUD(UUIHUDWidget* HUD);

	void UpdateStatistics() const;

	UFUNCTION()
	void OnAddScore(int32 getScore);

	UFUNCTION()
	void OnAddFireShots();

	UFUNCTION()
	void OnAddHitCount();		// hit판정시 delegate 실행

	UFUNCTION()
	void OnAddKill();			// kill판정시 delegate 실행

	UFUNCTION()
	void OnAddHeadshotCount();	// headshot판정시 delegate 실행

	int32 GetScore() const;

	int32 GetAccuracy() const;

	int32 GetKillCount() const;

	int32 GetShotsCount() const;

	int32 GetHeadshotRate() const;

protected:

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	void calcAccuracy();

private:

	FOnUpdateScore mOnUpdateScore;
	FOnUpdateAccuray mOnUpdateAccuracy;
	UPROPERTY()
	USRStatistics* mStatistics;

	// weapon
	int32 mHits;
	int32 mFireShots;		// result ui에서 사용
	int32 mAccuracy;		// hud, result ui에서 사용
	int32 mKills;			// result ui에서 사용.
	int32 mHeadshotsCount;	// result ui에서 사용.
	EWeaponType mWeaponType;

	// game
	int32 mScores;			// hud, result ui에서 사용
	EGameModeType mModeType;
	EGameType mGameType;

};
