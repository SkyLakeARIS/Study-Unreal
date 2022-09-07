// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include <vector>
#include <fstream>
#include "Version.h"
#include "UObject/NoExportTypes.h"
#include "SRStatistics.generated.h"

enum class EGameModeType : uint8;
enum class EGameType : uint8;
enum class EWeaponType : uint8;

USTRUCT(BlueprintType)
struct FWeaponStatistics
{
	GENERATED_BODY()
	FWeaponStatistics()
	{
		FireShots = 0;
		Hits = 0;
		Headshots = 0;
		Kills = 0;
		Selected = 0;
	}
	void operator=(const FWeaponStatistics& other)
	{
		FireShots = other.FireShots;
		Kills = other.Kills;
		Headshots = other.Headshots;
		Hits = other.Hits;
		Selected = other.Selected;
	}
	FWeaponStatistics& operator+=(const FWeaponStatistics& other)
	{
		FireShots += other.FireShots;
		Headshots += other.Headshots;
		Hits += other.Hits;
		Kills += other.Kills;
		Selected += other.Selected;
		return *this;
	}
	uint32 FireShots;
	uint32 Hits;
	uint32 Kills;
	uint32 Headshots;
	uint32 Selected;
};


/**
 *  유저의 통계 데이터를 파일에 읽고 쓰는 클래스입니다.
 *	SRPlayerState 클래스에서 데이터를 업데이트합니다.
 */
UCLASS()
class VERSION_API USRStatistics : public UObject
{
	GENERATED_BODY()
public:

	USRStatistics();

	bool LoadStats();

	bool SaveStats();

	void UpdateWeapon(const EWeaponType selectedWeapon, const FWeaponStatistics& stats);

	void UpdateGames(const EGameType selectedGameMode);

	void UpdateScores(EGameModeType EGameModeType, const uint32& score);

	FWeaponStatistics GetWeaponStats(EWeaponType weapon) const;

	uint32 GetGameStats(EGameType game) const;

	uint32 GetScoresStats(EGameModeType mode) const;

	uint32 GetHeadshotRate(EWeaponType weapon) const;

	uint32 GetAccuracy(EWeaponType weapon) const;

	bool IsLoaded() const;

private:

	void createDataFile();

	void parseWeapons(std::ifstream& fs, FWeaponStatistics& weapon);

	void parseGames(std::ifstream& fs);

	void parseScore(std::ifstream& fs);

private:
	// stats data file path
	const char* PATH = "..\\dplayer.srdata";
	bool mbIsLoaded;	// 파일에서 데이터를 불러오면 true가 됩니다.
	bool mbFail;		// 잘못된 데이터 전달로 인해 저장하면 안될 때 true입니다.

	// weapons
	FWeaponStatistics mAssultRifle;
	FWeaponStatistics mHandGun;
	FWeaponStatistics mSniperRifle;

	// game modes
	uint32 mSelectedBattlefield;
	uint32 mSelectedRainbowSix;
	uint32 mSelectedEscapeFromTarkov;

	// scores
	uint32 mStaticShortRangeHighScore;
	uint32 mStaticMidRangeHighScore;
	uint32 mStaticLongRangeHighScore;
	uint32 mMovableShortRangeHighScore;
	uint32 mMovableMidRangeHighScore;
	uint32 mMovableLongRangeHighScore;
};
