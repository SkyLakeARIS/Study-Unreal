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
}

void ASRPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	mOnUpdateScore.Unbind();
	mOnUpdateAccuracy.Unbind();
}

void ASRPlayerState::BindHUD(UUIHUDWidget* HUD)
{
	mOnUpdateScore.BindUObject(HUD, &UUIHUDWidget::UpdateScore);
	mOnUpdateAccuracy.BindUObject(HUD, &UUIHUDWidget::UpdateAccuracy);
}

// 한 게임동안 기록된 데이터들을 통계 클래스에 업데이트하고 파일에 저장합니다.
void ASRPlayerState::UpdateStatistics() const
{
	FWeaponStatistics weaponStats;
	weaponStats.FireShots = mFireShots;
	weaponStats.Hits = mHits;
	weaponStats.Kills = mKills;
	weaponStats.Headshots = mHeadshotsCount;
	weaponStats.Selected = 1;
	mStatistics->UpdateWeapon(mWeaponType, weaponStats);
	mStatistics->UpdateGames(mGameType);
	mStatistics->UpdateScores(mModeType, mScores);

	mStatistics->SaveStats();
}

void ASRPlayerState::OnAddScore(int32 getScore)
{
	mScores += getScore;
	mOnUpdateScore.Execute(mScores);
}

void ASRPlayerState::OnAddFireShots()
{
	mFireShots += 1;
	calcAccuracy();
}

void ASRPlayerState::OnAddHitCount()
{
	mHits += 1;
	calcAccuracy();
}

void ASRPlayerState::OnAddKill()
{
	mKills += 1;
}

void ASRPlayerState::OnAddHeadshotCount()
{
	mHeadshotsCount += 1;
}

// UI->character 게임 데이터가 세팅되면 데이터를 받아서 초기화합니다.
void ASRPlayerState::initialize(EWeaponType weapon, EGameType game, EGameModeType mode)
{
	checkf(weapon != EWeaponType::NONE, TEXT("EWeaponType이 none입니다."));
	checkf(game != EGameType::NONE, TEXT("EGameType이 none입니다."));
	checkf(mode != EGameModeType::None, TEXT("EGameModeType이 none입니다."));

	mWeaponType = weapon;
	mGameType = game;
	mModeType = mode;

	mStatistics = NewObject<USRStatistics>(this, USRStatistics::StaticClass());
	mStatistics->LoadStats();
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
	return (mHeadshotsCount / (float)(mHits))*100.0f;
}

// 맞춘 수 / 발사한 총알 수로 계산됩니다. (소숫점은 절삭합니다.)
void ASRPlayerState::calcAccuracy()
{
	mAccuracy = (mHits/(float)mFireShots)*100.0f;
	mOnUpdateAccuracy.Execute(mAccuracy);
}
