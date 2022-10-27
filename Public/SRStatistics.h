// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
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


/*
 *  ������ ��� �����͸� ���Ͽ� �а� ���� Ŭ�����Դϴ�.
 *	SRPlayerState Ŭ�������� �����͸� ������Ʈ�մϴ�.  : public UObject 
 */
//UCLASS()
class VERSION_API USRStatistics final
{
//	GENERATED_BODY()
public:

	USRStatistics() = delete;
	~USRStatistics() = delete;


	static bool LoadStats();
	static bool SaveStats();

	static void UpdateWeapon(const EWeaponType selectedWeapon, const FWeaponStatistics& stats);
	static void UpdateGames(const EGameType selectedGameMode);
	static void UpdateScores(EGameModeType EGameModeType, const uint32& score);

	/*
	 *  setter
	 */
	static void Initialize();

	/*
	 *  getter
	 */
	static FWeaponStatistics GetWeaponStats(EWeaponType weapon) ;
	static uint32 GetGameStats(EGameType game) ;
	static uint32 GetScoresStats(EGameModeType mode) ;
	static uint32 GetHeadshotRate(EWeaponType weapon) ;
	static uint32 GetAccuracy(EWeaponType weapon) ;


	static bool IsLoaded();

private:

	static void createDataFile();

	/*
	 *  parse data
	 */
	static bool parseWeapons(std::ifstream& fs, FWeaponStatistics& weapon);
	static bool parseGames(std::ifstream& fs);
	static bool parseScore(std::ifstream& fs);

private:
	/*
	 *  class info
	 */
	static const char* PATH;
	static bool mbIsInitialize;	// �̹� �ʱ�ȭ�� �������� �Ǵ��մϴ�.
	static bool mbIsLoaded;		// ���Ͽ��� �����͸� �ҷ����� true�� �˴ϴ�.
	static bool mbFail;			// �߸��� ������ ���޷� ���� �����ϸ� �ȵ� �� true�Դϴ�.

	// weapons
	static FWeaponStatistics mAssultRifle;
	static FWeaponStatistics mHandGun;
	static FWeaponStatistics mSniperRifle;

	// game modes
	static uint32 mSelectedBattlefield;
	static uint32 mSelectedRainbowSix;
	static uint32 mSelectedEscapeFromTarkov;

	// scores
	static uint32 mStaticShortRangeHighScore;
	static uint32 mStaticMidRangeHighScore;
	static uint32 mStaticLongRangeHighScore;
	static uint32 mMovableShortRangeHighScore;
	static uint32 mMovableMidRangeHighScore;
	static uint32 mMovableLongRangeHighScore;
};
