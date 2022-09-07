// Fill out your copyright notice in the Description page of Project Settings.
#include "SRStatistics.h"
#include <string>
#include "GameModeData.h"


USRStatistics::USRStatistics()
{
	mSelectedBattlefield = 0;
	mSelectedRainbowSix = 0;
	mSelectedEscapeFromTarkov = 0;

	// scores
	mStaticShortRangeHighScore = 0;
	mStaticMidRangeHighScore = 0;
	mStaticLongRangeHighScore = 0;
	mMovableShortRangeHighScore = 0;
	mMovableMidRangeHighScore = 0;
	mMovableLongRangeHighScore = 0;

	mbIsLoaded = false;
	mbFail = false;
}

bool USRStatistics::LoadStats()
{
	// file�� ������ ���� ���� �� ���� ä���
	std::ifstream fs(PATH);

	if (!fs.is_open())
	{
		enum { EQUALS = 0 };
		// �ʱ⿡ ������ ������ ������ ���� ���� ��, �ٽ� �ε��մϴ�.
		if (std::strcmp(strerror(errno), "No such file or directory") == EQUALS)
		{
			createDataFile();
			fs.clear();
			fs.open(PATH);
		}
		else
		{
			checkf(false, TEXT("���� ���¿� �����߽��ϴ�.: %hs"), strerror(errno));
			// ������ ���� �������� Ȯ�� �� ���� ����
			return false;
		}
	}

	// file�� ������ �Ľ� ����
	std::string header;
	std::string garbage;
	enum { HEADER_SIZE = 32, GARBAGE_SIZE = 8 };
	header.reserve(HEADER_SIZE);
	garbage.reserve(GARBAGE_SIZE);

	do
	{
		std::getline(fs, header);
		UE_LOG(LogTemp, Warning, TEXT("%hs"), header.c_str());

		enum { EQUALS = 0 };
		if (header.compare("Weapons") == EQUALS)
		{
			// { �� �����ϴ�.
			std::getline(fs, garbage);
			UE_LOG(LogTemp, Warning, TEXT("trunc : %hs"), garbage.c_str());

			std::string weapon;
			weapon.reserve(HEADER_SIZE);
			while (true)
			{
				std::getline(fs, weapon);
				UE_LOG(LogTemp, Warning, TEXT("%hs"), weapon.c_str());

				if (weapon.compare("}") == EQUALS)
				{
					// weapons �κ� �Ľ��� �������Ƿ� while���� Ż���մϴ�.
					goto PARSE_END;
				}

				if (weapon.compare("AssultRifle") == EQUALS)
				{
					parseWeapons(fs, mAssultRifle);
				}
				else if (weapon.compare("HandGun") == EQUALS)
				{
					parseWeapons(fs, mHandGun);
				}
				else if (weapon.compare("SniperRifle") == EQUALS)
				{
					parseWeapons(fs, mSniperRifle);
				}
				else
				{
					// �� �ܴ� ������ �����մϴ�.
					checkf(false, TEXT("������ �Ľ� ������ ������ �߻��߽��ϴ�. : %hs"), weapon.c_str());
				}
				weapon.clear();
			}
		}
		else if (header.compare("Games") == EQUALS)
		{
			parseGames(fs);
		}
		else if (header.compare("Scores") == EQUALS)
		{
			parseScore(fs);
		}
		else
		{
			checkf(false, TEXT("������ ������ �߸��Ǿ����ϴ�. : %hs"), header.c_str());
		}
		PARSE_END:
		header.clear();
	} while (!fs.eof());
	// ���� �бⰡ �����ϸ� true�̱� ������ eof ���� fs ��ü�� �����ϴ� ���� ���ٴ� ������ ����.

	// ���������� close.
	fs.close();
	UE_LOG(LogTemp, Warning, TEXT("success load stats! - file closed"));
	mbIsLoaded = true;
	return true;
}

bool USRStatistics::SaveStats()
{
	if(mbFail) // true�� �������� �ʽ��ϴ�.
	{
		UE_LOG(LogTemp, Error, TEXT("Fail value is true. can not save new statistics"));
		return false;
	}

	const char* statsDataFormat("%s\n%s\n%s");

	const char* weaponStatsFormat("Weapons\n{\n%s\n%s\n%s\n}");

	const char* gunStatsFormat(
		"%s\n{\nFireshots : %u\nHits : %u\nKills : %u\nHeadshots : %u\nSelected : %u\n}");

	const char* gameStatsString(
		"Games\n{\nSelectedBattlefield : %u\nSelectedRainbowSix : %u\nSelectedEscapeFromTarkov : %u\n}");

	const char* scoreStatsString(
		"Scores\n{\nStaticShortRangeHighScore : %u\nStaticMidRangeHighScore : %u\nStaticLongRangeHighScore : %u\nMovableShortRangeHighScore : %u\nMovableMidRangeHighScore : %u\nMovableLongRangeHighScore : %u\n}");
	char arStats[256];
	char hgStats[256];
	char srStats[256];
	char weaponStats[512];
	char gameStats[256];
	char scoreStats[256];
	char statsFinal[1024];

	sprintf(arStats, gunStatsFormat, "AssultRifle", mAssultRifle.FireShots, mAssultRifle.Hits, mAssultRifle.Kills, mAssultRifle.Headshots, mAssultRifle.Selected);
	sprintf(hgStats, gunStatsFormat, "HandGun", mHandGun.FireShots, mHandGun.Hits, mHandGun.Kills, mHandGun.Headshots, mHandGun.Selected);
	sprintf(srStats, gunStatsFormat, "SniperRifle", mSniperRifle.FireShots, mSniperRifle.Hits, mSniperRifle.Kills, mSniperRifle.Headshots, mSniperRifle.Selected);

	sprintf(weaponStats, weaponStatsFormat, arStats, hgStats, srStats);

	sprintf(gameStats, gameStatsString, mSelectedBattlefield, mSelectedRainbowSix, mSelectedEscapeFromTarkov);
	sprintf(scoreStats, scoreStatsString, mStaticShortRangeHighScore, mStaticMidRangeHighScore, mStaticLongRangeHighScore, mMovableShortRangeHighScore, mMovableMidRangeHighScore, mMovableLongRangeHighScore);

	sprintf(statsFinal, statsDataFormat, weaponStats, gameStats, scoreStats);


	std::ofstream os;
	os.open(PATH, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
	if (os.is_open())
	{
		os.write(statsFinal, strlen(statsFinal));
		os.flush();
		os.close();
		UE_LOG(LogTemp, Warning, TEXT("success save stats! - file closed"));
	}
	else
	{
		checkf(false, TEXT("%hs"), strerror(errno));
		return false;
	}
	return true;
}

void USRStatistics::UpdateWeapon(const EWeaponType selectedWeapon, const FWeaponStatistics& stats)
{
	mbFail = false;
	switch (selectedWeapon)
	{
		case EWeaponType::AR:
			mAssultRifle += stats;
			break;
		case EWeaponType::HG:
			mHandGun += stats;
			break;
		case EWeaponType::SR:
			mSniperRifle += stats;
			break;
		default:
			mbFail = true;
			checkf(false, TEXT("�߸��� enumŸ���Դϴ�."));
			break;
	}
}

void USRStatistics::UpdateGames(const EGameType selectedGameMode)
{
	mbFail = false;
	switch (selectedGameMode)
	{
		case EGameType::Battlefield:
			mSelectedBattlefield += 1;
			break;
		case EGameType::RainbowSix:
			mSelectedRainbowSix += 1;
			break;
		case EGameType::Tarkov:
			mSelectedEscapeFromTarkov += 1;
			break;
		default:
			mbFail = true;
			checkf(false, TEXT("�߸��� enumŸ���Դϴ�."));
			break;
	}
}

void USRStatistics::UpdateScores(EGameModeType modeType, const uint32& score)
{
	mbFail = false;
	switch (modeType)
	{
		case EGameModeType::Static_ShortRange:
			mStaticShortRangeHighScore = (mStaticShortRangeHighScore < score) ? score : mStaticShortRangeHighScore;
			break;
		case EGameModeType::Static_MidRange:
			mStaticMidRangeHighScore = (mStaticMidRangeHighScore < score) ? score : mStaticMidRangeHighScore;
			break;
		case EGameModeType::Static_LongRange:
			mStaticLongRangeHighScore = (mStaticLongRangeHighScore < score) ? score : mStaticLongRangeHighScore;
			break;
		case EGameModeType::Movable_ShortRange:
			mMovableShortRangeHighScore = (mMovableShortRangeHighScore < score) ? score : mMovableShortRangeHighScore;
			break;
		case EGameModeType::Movable_MidRange:
			mMovableMidRangeHighScore = (mMovableMidRangeHighScore < score) ? score : mMovableMidRangeHighScore;
			break;
		case EGameModeType::Movable_LongRange:
			mMovableLongRangeHighScore = (mMovableLongRangeHighScore < score) ? score : mMovableLongRangeHighScore;
			break;
		default:
			mbFail = true;
			checkf(false, TEXT("�߸��� enumŸ���Դϴ�."));
			break;
	}
}

FWeaponStatistics USRStatistics::GetWeaponStats(EWeaponType weapon) const
{
	switch (weapon)
	{
		case EWeaponType::AR:
			return mAssultRifle;
		case EWeaponType::HG:
			return mHandGun;
		case EWeaponType::SR:
			return mSniperRifle;
		default:
			FWeaponStatistics wrong;
			checkf(false, TEXT("�߸��� enumŸ���Դϴ�."));
			return wrong;
	}
}

uint32 USRStatistics::GetGameStats(EGameType game) const
{
	switch (game)
	{
		case EGameType::Battlefield:
			return mSelectedBattlefield;
		case EGameType::RainbowSix:
			return mSelectedRainbowSix;
		case EGameType::Tarkov:
			return mSelectedEscapeFromTarkov;
		default:
			checkf(false, TEXT("�߸��� enumŸ���Դϴ�."));
			return 0;
	}
}

uint32 USRStatistics::GetScoresStats(EGameModeType mode) const
{
	switch (mode)
	{
		case EGameModeType::Static_ShortRange:
			return mStaticShortRangeHighScore;
		case EGameModeType::Static_MidRange:
			return mStaticMidRangeHighScore;
		case EGameModeType::Static_LongRange:
			return mStaticLongRangeHighScore;
		case EGameModeType::Movable_ShortRange:
			return mMovableShortRangeHighScore;
		case EGameModeType::Movable_MidRange:
			return mMovableMidRangeHighScore;
		case EGameModeType::Movable_LongRange:
			return mMovableLongRangeHighScore;
		default:
			checkf(false, TEXT("�߸��� enumŸ���Դϴ�."));
			return 0;
	}
}

uint32 USRStatistics::GetHeadshotRate(EWeaponType weapon) const
{
	const FWeaponStatistics* stats = nullptr;
	switch (weapon)
	{
		case EWeaponType::AR:
			stats = &mAssultRifle;
			break;
		case EWeaponType::HG:
			stats = &mHandGun;
			break;
		case EWeaponType::SR:
			stats = &mSniperRifle;
			break;
		default:
			checkf(false, TEXT("�߸��� enumŸ���Դϴ�."));
			const FWeaponStatistics wrong;
			stats = &wrong;
	}

	enum{ZERO};
	if(stats->Hits == ZERO)
	{
		return ZERO;
	}
	return (stats->Headshots / (float)stats->Hits) * 100.0f;
}

uint32 USRStatistics::GetAccuracy(EWeaponType weapon) const
{
	const FWeaponStatistics* stats = nullptr;
	switch (weapon)
	{
		case EWeaponType::AR:
			stats = &mAssultRifle;
			break;
		case EWeaponType::HG:
			stats = &mHandGun;
			break;
		case EWeaponType::SR:
			stats = &mSniperRifle;
			break;
		default:
			checkf(false, TEXT("�߸��� enumŸ���Դϴ�."));
			const FWeaponStatistics wrong;
			stats = &wrong;
	}

	enum { ZERO };
	if (stats->FireShots == ZERO)
	{
		return ZERO;
	}
	return (stats->Hits / (float)stats->FireShots) * 100.0f;
}

bool USRStatistics::IsLoaded() const
{
	return mbIsLoaded;
}


void USRStatistics::createDataFile()
{
	// �ϴ� ���� ���� ������ �ش� �Լ��� ȣ���մϴ�.
	SaveStats();
}

void USRStatistics::parseWeapons(std::ifstream& fs, FWeaponStatistics& weapon)
{
	std::string line;
	std::string garbage;
	enum { DATA_SIZE = 32, GARBAGE_SIZE = 8 };
	line.reserve(DATA_SIZE);
	garbage.reserve(GARBAGE_SIZE);

	// { �� �����ϴ�.
	std::getline(fs, garbage);
	UE_LOG(LogTemp, Warning, TEXT("trunc : %hs"), garbage.c_str());

	while (true)
	{
		std::getline(fs, line);
		UE_LOG(LogTemp, Warning, TEXT("%hs"), line.c_str());

		// �� ����� �Ľ��� �������ϴ�.
		enum { EQUALS = 0 };
		if (line.compare("}") == EQUALS)
		{
			UE_LOG(LogTemp, Warning, TEXT("parseWeapons end"));

			break;
		}
		// : �� �������� ���ڿ��� �и��մϴ�.
		char statName[32];
		uint32 statData = 0;
		std::string a;
		sscanf(line.c_str(), "%s %*s %u", statName, &statData);

		if (std::strcmp(statName, "Fireshots") == EQUALS)
		{
			weapon.FireShots = statData;
		}
		else if (std::strcmp(statName, "Hits") == EQUALS)
		{
			weapon.Hits = statData;
		}
		else if (std::strcmp(statName, "Headshots") == EQUALS)
		{
			weapon.Headshots = statData;
		}
		else if (std::strcmp(statName, "Kills")== EQUALS)
		{
			weapon.Kills = statData;
		}
		else if (std::strcmp(statName, "Selected") == EQUALS)
		{
			weapon.Selected = statData;
		}
		else
		{
			checkf(false, TEXT("���� �����Ͱ� �߸��Ǿ����ϴ�. : %hs"), statName);
		}
		line.clear();
	}
}

void USRStatistics::parseGames(std::ifstream& fs)
{
	std::string line;
	std::string garbage;
	enum { DATA_SIZE = 64, GARBAGE_SIZE = 8 };
	line.reserve(DATA_SIZE);
	garbage.reserve(GARBAGE_SIZE);

	// { �� �����ϴ�.
	std::getline(fs, garbage);
	UE_LOG(LogTemp, Warning, TEXT("trunc : %hs"), garbage.c_str());

	while (true)
	{
		std::getline(fs, line);
		UE_LOG(LogTemp, Warning, TEXT("%hs"), line.c_str());

		// �� ����� �Ľ��� �������ϴ�.
		enum { EQUALS = 0 };
		if (line.compare("}") == EQUALS)
		{
			UE_LOG(LogTemp, Warning, TEXT("parseGames end"));
			break;
		}

		// : �� �������� ���ڿ��� �и��մϴ�.
		char statName[32];
		uint32 statData = 0;
		sscanf(line.c_str(), "%s %*s %u", statName, &statData);

		if (std::strcmp(statName, "SelectedBattlefield") == EQUALS)
		{
			mSelectedBattlefield = statData;
		}
		else if (std::strcmp(statName, "SelectedRainbowSix") == EQUALS)
		{
			mSelectedRainbowSix = statData;
		}
		else if (std::strcmp(statName, "SelectedEscapeFromTarkov") == EQUALS)
		{
			mSelectedEscapeFromTarkov = statData;
		}
		else
		{
			checkf(false, TEXT("���� �����Ͱ� �߸��Ǿ����ϴ�. : %hs"), statName);
		}

		line.clear();
	}
}

void USRStatistics::parseScore(std::ifstream& fs)
{
	std::string line;
	std::string garbage;
	enum { DATA_SIZE = 64, GARBAGE_SIZE = 8 };
	line.reserve(DATA_SIZE);
	garbage.reserve(GARBAGE_SIZE);

	// { �� �����ϴ�.
	std::getline(fs, garbage);
	UE_LOG(LogTemp, Warning, TEXT("trunc : %hs"), garbage.c_str());

	while (true)
	{
		std::getline(fs, line);
		UE_LOG(LogTemp, Warning, TEXT("%hs"), line.c_str());


		// �� ����� �Ľ��� �������ϴ�.
		enum { EQUALS = 0 };
		if (line.compare("}") == EQUALS)
		{
			UE_LOG(LogTemp, Warning, TEXT("parse scores end"));
			break;
		}

		// : �� �������� ���ڿ��� �и��մϴ�.
		char statName[32];
		uint32 statData = 0;
		sscanf(line.c_str(), "%s %*s %u", statName, &statData);


		if (std::strcmp(statName, "StaticShortRangeHighScore") == EQUALS)
		{
			mStaticShortRangeHighScore = statData;
		}
		else if (std::strcmp(statName, "StaticMidRangeHighScore") == EQUALS)
		{
			mStaticMidRangeHighScore = statData;
		}
		else if (std::strcmp(statName, "StaticLongRangeHighScore") == EQUALS)
		{
			mStaticLongRangeHighScore = statData;
		}
		else if (std::strcmp(statName, "MovableShortRangeHighScore") == EQUALS)
		{
			mMovableShortRangeHighScore = statData;
		}
		else if (std::strcmp(statName, "MovableMidRangeHighScore") == EQUALS)
		{
			mMovableMidRangeHighScore = statData;
		}
		else if (std::strcmp(statName, "MovableLongRangeHighScore") == EQUALS)
		{
			mMovableLongRangeHighScore = statData;
		}
		else
		{
			checkf(false, TEXT("���� �����Ͱ� �߸��Ǿ����ϴ�. : %hs"), statName);
		}
		line.clear();
	}
}