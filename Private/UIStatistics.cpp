// Fill out your copyright notice in the Description page of Project Settings.
#include "UIStatistics.h"
#include <string>
#include "SRProjectile.h"
#include "SRStatistics.h"
#include "Components/TextBlock.h"

void UUIStatistics::NativeConstruct()
{
	Super::NativeConstruct();
	// ar stats
	mARFireshots = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_AR_Fireshots")));
	mARHits = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_AR_Hits")));
	mARAccuracy = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_AR_Accuracy")));
	mARHeadshots = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_AR_Headshots")));
	mARHeadshotRate = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_AR_HeadshotRate")));
	mARKills = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_AR_Kills")));
	mARSelected = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_AR_Selected")));

	// hg stats
	mHGFireshots = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_HG_Fireshots")));
	mHGHits = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_HG_Hits")));
	mHGAccuracy = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_HG_Accuracy")));
	mHGHeadshots = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_HG_Headshots")));
	mHGHeadshotRate = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_HG_HeadshotRate")));
	mHGKills = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_HG_Kills")));;
	mHGSelected = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_HG_Selected")));

	// sr stats
	mSRFireshots = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_SR_Fireshots")));
	mSRHits = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_SR_Hits")));
	mSRAccuracy = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_SR_Accuracy")));
	mSRHeadshots = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_SR_Headshots")));
	mSRHeadshotRate = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_SR_HeadshotRate")));
	mSRKills = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_SR_Kills")));
	mSRSelected = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_SR_Selected")));

	// game stats
	mSelectedBattlefield = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_Selected_Battlefield")));
	mSelectedRainbowSix = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_Selected_RainbowSix")));
	mSelectedEscapeFromTarkov = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_Selected_EscapeFromTarkov")));

	// mode score stats
	mStaticShortRangeScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_Score_StaticShort")));
	mStaticMidRangeScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_Score_StaticMid")));
	mStaticLongRangeScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_Score_StaticLong")));
	mMovableShortRangeScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_Score_MovableShort")));
	mMovableMidRangeScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_Score_MovableMid")));
	mMovableLongRangeScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("Display_Score_MovableLong")));
}

// (로드되어 있지 않으면) 통계를 로드하고 데이터를 받아와 UI에 표시합니다.
void UUIStatistics::UpdateStatistics()
{
	if (!USRStatistics::IsLoaded())
	{
		USRStatistics::LoadStats();
	}

	FWeaponStatistics weapon = USRStatistics::GetWeaponStats(EWeaponType::AR);

	mARFireshots->SetText(FText::FromString(FString(std::to_string(weapon.FireShots).c_str())));
	mARHits->SetText(FText::FromString(FString(std::to_string(weapon.Hits).c_str())));
	mARAccuracy->SetText(FText::FromString(FString(std::to_string(USRStatistics::GetAccuracy(EWeaponType::AR)).c_str() + FString("%"))));
	mARHeadshots->SetText(FText::FromString(FString(std::to_string(weapon.Headshots).c_str())));
	mARHeadshotRate->SetText(FText::FromString(FString(std::to_string(USRStatistics::GetHeadshotRate(EWeaponType::AR)).c_str()) + FString("%")));
	mARKills->SetText(FText::FromString(FString(std::to_string(weapon.Kills).c_str())));
	mARSelected->SetText(FText::FromString(FString(std::to_string(weapon.Selected).c_str())));

	weapon = USRStatistics::GetWeaponStats(EWeaponType::HG);

	mHGFireshots->SetText(FText::FromString(FString(std::to_string(weapon.FireShots).c_str())));
	mHGHits->SetText(FText::FromString(FString(std::to_string(weapon.Hits).c_str())));
	mHGAccuracy->SetText(FText::FromString(FString(std::to_string(USRStatistics::GetAccuracy(EWeaponType::HG)).c_str() + FString("%"))));
	mHGHeadshots->SetText(FText::FromString(FString(std::to_string(weapon.Headshots).c_str())));
	mHGHeadshotRate->SetText(FText::FromString(FString(std::to_string(USRStatistics::GetHeadshotRate(EWeaponType::HG)).c_str()) + FString("%")));
	mHGKills->SetText(FText::FromString(FString(std::to_string(weapon.Kills).c_str())));
	mHGSelected->SetText(FText::FromString(FString(std::to_string(weapon.Selected).c_str())));

	weapon = USRStatistics::GetWeaponStats(EWeaponType::SR);

	mSRFireshots->SetText(FText::FromString(FString(std::to_string(weapon.FireShots).c_str())));
	mSRHits->SetText(FText::FromString(FString(std::to_string(weapon.Hits).c_str())));
	mSRAccuracy->SetText(FText::FromString(FString(std::to_string(USRStatistics::GetAccuracy(EWeaponType::SR)).c_str() + FString("%"))));
	mSRHeadshots->SetText(FText::FromString(FString(std::to_string(weapon.Headshots).c_str())));
	mSRHeadshotRate->SetText(FText::FromString(FString(std::to_string(USRStatistics::GetHeadshotRate(EWeaponType::SR)).c_str()) + FString("%")));
	mSRKills->SetText(FText::FromString(FString(std::to_string(weapon.Kills).c_str())));
	mSRSelected->SetText(FText::FromString(FString(std::to_string(weapon.Selected).c_str())));

	mSelectedBattlefield->SetText(FText::FromString(FString(std::to_string(USRStatistics::GetGameStats(EGameType::Battlefield)).c_str())));
	mSelectedRainbowSix->SetText(FText::FromString(FString(std::to_string(USRStatistics::GetGameStats(EGameType::RainbowSix)).c_str())));
	mSelectedEscapeFromTarkov->SetText(FText::FromString(FString(std::to_string(USRStatistics::GetGameStats(EGameType::Tarkov)).c_str())));

	mStaticShortRangeScore->SetText(FText::FromString(FString(std::to_string(USRStatistics::GetScoresStats(EGameModeType::Static_ShortRange)).c_str())));
	mStaticMidRangeScore->SetText(FText::FromString(FString(std::to_string(USRStatistics::GetScoresStats(EGameModeType::Static_MidRange)).c_str())));
	mStaticLongRangeScore->SetText(FText::FromString(FString(std::to_string(USRStatistics::GetScoresStats(EGameModeType::Static_LongRange)).c_str())));
	mMovableShortRangeScore->SetText(FText::FromString(FString(std::to_string(USRStatistics::GetScoresStats(EGameModeType::Movable_ShortRange)).c_str())));
	mMovableMidRangeScore->SetText(FText::FromString(FString(std::to_string(USRStatistics::GetScoresStats(EGameModeType::Movable_MidRange)).c_str())));
	mMovableLongRangeScore->SetText(FText::FromString(FString(std::to_string(USRStatistics::GetScoresStats(EGameModeType::Movable_LongRange)).c_str())));
}
