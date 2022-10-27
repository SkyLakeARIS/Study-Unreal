// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "GameFramework/PlayerState.h"
#include "SRPlayerState.generated.h"

DECLARE_DELEGATE_OneParam(FOnUpdateScore, int32)
DECLARE_DELEGATE_OneParam(FOnUpdateAccuray, int32)

enum class EGameType : uint8;
enum class EWeaponType : uint8;
enum class EGameModeType : uint8;
class UUIHUDWidget;

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

	void UpdateToStatistics() const;

	UFUNCTION()
	void OnAddScore(int32 getScore);

	UFUNCTION()
	void OnAddFireShots();

	UFUNCTION()
	void OnAddHitCount();		// hit������ delegate ����

	UFUNCTION()
	void OnAddKill();			// kill������ delegate ����

	UFUNCTION()
	void OnAddHeadshotCount();	// headshot������ delegate ����

	/*
	 *  setter
	 */
	void SetRecordMode(bool isRecordable);

	void Initialize(EWeaponType weapon, EGameType game, EGameModeType mode, UUIHUDWidget* HUD);

	/*
	 *  getter
	 */
	// playerState �θ�Ŭ������ Score�� getter setter�� �־ �װ� �ᵵ��.
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
	/*
	 *  delegates
	 */
	FOnUpdateScore mOnUpdateScore;
	FOnUpdateAccuray mOnUpdateAccuracy;

	/*
	 *  weapon stats
	 */
	int32 mHits;
	int32 mFireShots;		// result ui���� ���
	int32 mAccuracy;		// hud, result ui���� ���
	int32 mKills;			// result ui���� ���.
	int32 mHeadshotsCount;	// result ui���� ���.
	EWeaponType mWeaponType;

	/*
	 * score stats
	 */ 
	int32 mScores;			// hud, result ui���� ���

	/*
	 *  game mode data
	 */
	EGameModeType mModeType;
	EGameType mGameType;

	/*
	 *  only record statistics in main game time
	 */
	bool mbRecordable;
};
