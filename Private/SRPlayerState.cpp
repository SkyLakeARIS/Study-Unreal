// Fill out your copyright notice in the Description page of Project Settings.


#include "SRPlayerState.h"

#include "UIHUDWidget.h"

ASRPlayerState::ASRPlayerState()
{
	mScore = 0;
	mAccuracy = 0.0f;
	mHits = 0;
	mFireShots = 0;
	mKill = 0;
}

void ASRPlayerState::BindHUD(UUIHUDWidget* HUD)
{
	mOnUpdateScore.BindUObject(HUD, &UUIHUDWidget::UpdateScore);
	mOnUpdateAccuracy.BindUObject(HUD, &UUIHUDWidget::UpdateAccuracy);
	//mOnupdateKillCount.BindUObject(HUD, &UUIHUDWidget::UpdateKillCount);
}

void ASRPlayerState::OnAddScore(int32 getScore)
{
	mScore += getScore;
	mOnUpdateScore.Execute(mScore);
	UE_LOG(LogTemp, Warning, TEXT("score added: %d"), getScore);

}

void ASRPlayerState::OnAddFireShots(int32 shots)
{
	mFireShots += shots;
	calcAccuracy();
}

void ASRPlayerState::OnHitCount()
{
	mHits += 1;
	calcAccuracy();
	UE_LOG(LogTemp, Warning, TEXT("Accuracy : %f"), mAccuracy);
}

void ASRPlayerState::OnKill()
{
	mKill += 1;
}

int32 ASRPlayerState::GetScore()
{
	return mScore;
}

int32 ASRPlayerState::GetAccuracy()
{
	return mAccuracy;
}

int32 ASRPlayerState::GetKillCount()
{
	return mKill;
}

void ASRPlayerState::calcAccuracy()
{
	mAccuracy = (mHits/(float)mFireShots)*100.0f;
	mOnUpdateAccuracy.Execute(mAccuracy);
}
