// Fill out your copyright notice in the Description page of Project Settings.
#include "SRPlayerState.h"
#include "SRProjectile.h"
#include "SRStatistics.h"
#include "UIHUDWidget.h"

ASRPlayerState::ASRPlayerState()
{
	mScores = 0;
	mAccuracy = 0.0f;
	mHits = 0;
	mFireShots = 0;
	mKills = 0;
	mHeadshotsCount = 0;
	mbRecordable = false;
}

void ASRPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	mOnUpdateScore.Unbind();
	mOnUpdateAccuracy.Unbind();
}

// 한 게임동안 기록된 데이터들을 통계 클래스에 업데이트하고 파일에 저장합니다.
void ASRPlayerState::UpdateToStatistics() const
{
	FWeaponStatistics weaponStats;
	weaponStats.FireShots = mFireShots;
	weaponStats.Hits = mHits;
	weaponStats.Kills = mKills;
	weaponStats.Headshots = mHeadshotsCount;
	weaponStats.Selected = 1;
	USRStatistics::UpdateWeapon(mWeaponType, weaponStats);
	USRStatistics::UpdateGames(mGameType);
	USRStatistics::UpdateScores(mModeType, mScores);

	USRStatistics::SaveStats();
}

void ASRPlayerState::OnAddScore(int32 getScore)
{
	if(mbRecordable)
	{
		mScores += getScore;
		mOnUpdateScore.Execute(mScores);
	}
}

void ASRPlayerState::OnAddFireShots()
{
	if (mbRecordable)
	{
		mFireShots += 1;
		calcAccuracy();
	}
}

void ASRPlayerState::OnAddHitCount()
{
	if (mbRecordable)
	{
		mHits += 1;
		calcAccuracy();
	}
}

void ASRPlayerState::OnAddKill()
{
	if (mbRecordable)
	{
		mKills += 1;
	}
}

void ASRPlayerState::OnAddHeadshotCount()
{
	if (mbRecordable)
	{
		mHeadshotsCount += 1;
	}
}

// UI->character 게임 데이터가 세팅되면 데이터를 받아서 초기화합니다.
void ASRPlayerState::Initialize(eWeaponType weapon, eGameType game, eGameModeType mode, UUIHUDWidget* HUD)
{
	checkf(weapon != eWeaponType::NONE, TEXT("EWeaponType이 none입니다."));
	checkf(game != eGameType::NONE, TEXT("EGameType이 none입니다."));
	checkf(mode != eGameModeType::None, TEXT("EGameModeType이 none입니다."));

	mWeaponType = weapon;
	mGameType = game;
	mModeType = mode;

	mOnUpdateScore.BindUObject(HUD, &UUIHUDWidget::UpdateScore);
	mOnUpdateAccuracy.BindUObject(HUD, &UUIHUDWidget::UpdateAccuracy);

	USRStatistics::LoadStats();
}

int32 ASRPlayerState::GetScore() const
{
	return mScores;
}

int32 ASRPlayerState::GetAccuracy() const
{
	return mAccuracy;
}

int32 ASRPlayerState::GetKillCount() const
{
	return mKills;
}

int32 ASRPlayerState::GetShotsCount() const
{
	return mFireShots;
}

// 헤드샷 수 / 맞춘 수로 계산됩니다. (소숫점은 절삭합니다.)
int32 ASRPlayerState::GetHeadshotRate() const
{
	if(!mHits)
	{
		return 0;
	}
	return (mHeadshotsCount / (float)(mHits))*100.0f;
}

void ASRPlayerState::SetRecordMode(bool isRecordable)
{
	mbRecordable = isRecordable;
}

// 맞춘 수 / 발사한 총알 수로 계산됩니다. (소숫점은 절삭합니다.)
void ASRPlayerState::calcAccuracy()
{
	if(!mFireShots)
	{
		mAccuracy = 0;
	}
	else
	{
		mAccuracy = (mHits/(float)mFireShots)*100.0f;
	}
	mOnUpdateAccuracy.Execute(mAccuracy);
}
