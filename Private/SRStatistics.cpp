// Fill out your copyright notice in the Description page of Project Settings.
#include "SRStatistics.h"
#include <string>
#include "GameModeData.h"

 const char* USRStatistics::PATH = "..\\dplayer.srdata";
 bool USRStatistics::mbIsInitialize = false;	// 이미 초기화된 상태인지 판단합니다.
 bool USRStatistics::mbIsLoaded = false;		// 파일에서 데이터를 불러오면 true가 됩니다.
 bool USRStatistics::mbFail = false;			// 잘못된 데이터 전달로 인해 저장하면 안될 때 true입니다.

// weapons
 FWeaponStatistics USRStatistics::mAssultRifle = FWeaponStatistics();
 FWeaponStatistics USRStatistics::mHandGun = FWeaponStatistics();
 FWeaponStatistics USRStatistics::mSniperRifle = FWeaponStatistics();

// game modes
 uint32 USRStatistics::mSelectedBattlefield = 0;
 uint32 USRStatistics::mSelectedRainbowSix = 0;
 uint32 USRStatistics::mSelectedEscapeFromTarkov = 0;

// scores
 uint32 USRStatistics::mStaticShortRangeHighScore = 0;
 uint32 USRStatistics::mStaticMidRangeHighScore = 0;
 uint32 USRStatistics::mStaticLongRangeHighScore = 0;
 uint32 USRStatistics::mMovableShortRangeHighScore = 0;
 uint32 USRStatistics::mMovableMidRangeHighScore = 0;
 uint32 USRStatistics::mMovableLongRangeHighScore = 0;


void USRStatistics::Initialize()
{
	//checkf(mbIsInitialize == false, TEXT("클래스가 두번 초기화 되었습니다."));

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
	mbIsInitialize = true;
}

bool USRStatistics::LoadStats()
{
	// file이 없으면 파일 생성 후 내용 채우기
	std::ifstream fs(PATH);

	if (!fs.is_open())
	{
		enum { EQUALS = 0 };
		// 초기에 데이터 파일이 없으면 새로 생성 후, 다시 로드합니다.
		if (std::strcmp(strerror(errno), "No such file or directory") == EQUALS)
		{
			createDataFile();
			fs.clear();
			fs.open(PATH);
		}
		else
		{
			checkf(false, TEXT("파일 오픈에 실패했습니다.: %hs"), strerror(errno));
			return false;
		}
	}
	bool bSuccess = false;
	// file이 있으면 파싱 시작
	std::string header;
	std::string garbage;
	enum { HEADER_SIZE = 32, GARBAGE_SIZE = 8 };
	header.reserve(HEADER_SIZE);
	garbage.reserve(GARBAGE_SIZE);

	do
	{
		std::getline(fs, header);

		enum { EQUALS = 0 };
		if (header.compare("Weapons") == EQUALS)
		{
			// { 를 버립니다.
			std::getline(fs, garbage);

			std::string weapon;
			weapon.reserve(HEADER_SIZE);
			while (true)
			{
				std::getline(fs, weapon);

				if (weapon.compare("}") == EQUALS)
				{
					// weapons 부분 파싱이 끝났으므로 while문을 탈출합니다.
					goto PARSE_WEAPONS_END;
				}

				if (weapon.compare("AssultRifle") == EQUALS)
				{
					bSuccess = parseWeapons(fs, mAssultRifle);
				}
				else if (weapon.compare("HandGun") == EQUALS)
				{
					bSuccess = parseWeapons(fs, mHandGun);
				}
				else if (weapon.compare("SniperRifle") == EQUALS)
				{
					bSuccess = parseWeapons(fs, mSniperRifle);
				}
				else
				{
					// 그 외는 에러로 간주합니다.
					checkf(false, TEXT("데이터 파싱 과정에 문제가 발생했습니다. : %hs"), weapon.c_str());
					goto ERROR_END_PARSE;
				}
				weapon.clear();

				if(!bSuccess)
				{
					goto ERROR_END_PARSE;
				}
			}
		}
		else if (header.compare("Games") == EQUALS)
		{
			bSuccess = parseGames(fs);
		}
		else if (header.compare("Scores") == EQUALS)
		{
			bSuccess = parseScore(fs);
		}
		else
		{
			checkf(false, TEXT("데이터 포멧이 잘못되었습니다. : %hs"), header.c_str());
			goto ERROR_END_PARSE;
		}

		if (!bSuccess)
		{
			goto ERROR_END_PARSE;
		}

		PARSE_WEAPONS_END:

		header.clear();
	} while (!fs.eof());
	// 다음 읽기가 가능하면 true이기 때문에 eof 말고 fs 자체를 전달하는 것이 좋다는 정보도 있음.


	bSuccess = true;
	UE_LOG(LogTemp, Warning, TEXT("success load statistics! - file closed"));
	mbIsLoaded = true;

	ERROR_END_PARSE:
	// 명시적으로 close. 혹은 에러
	fs.close();
	return bSuccess;
}

bool USRStatistics::SaveStats()
{
	if(mbFail) // true면 저장하지 않습니다.
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
			checkf(false, TEXT("잘못된 enum타입입니다."));
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
			checkf(false, TEXT("잘못된 enum타입입니다."));
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
			checkf(false, TEXT("잘못된 enum타입입니다."));
			break;
	}
}

FWeaponStatistics USRStatistics::GetWeaponStats(EWeaponType weapon)
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
			checkf(false, TEXT("잘못된 enum타입입니다."));
			return wrong;
	}
}

uint32 USRStatistics::GetGameStats(EGameType game)
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
			checkf(false, TEXT("잘못된 enum타입입니다."));
			return 0;
	}
}

uint32 USRStatistics::GetScoresStats(EGameModeType mode) 
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
			checkf(false, TEXT("잘못된 enum타입입니다."));
			return 0;
	}
}

uint32 USRStatistics::GetHeadshotRate(EWeaponType weapon) 
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
			checkf(false, TEXT("잘못된 enum타입입니다."));
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

uint32 USRStatistics::GetAccuracy(EWeaponType weapon) 
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
			checkf(false, TEXT("잘못된 enum타입입니다."));
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

bool USRStatistics::IsLoaded() 
{
	return mbIsLoaded;
}


void USRStatistics::createDataFile()
{
	// 하는 일이 같기 때문에 해당 함수를 호출합니다.
	SaveStats();
}

bool USRStatistics::parseWeapons(std::ifstream& fs, FWeaponStatistics& weapon)
{
	std::string line;
	std::string garbage;
	enum { DATA_SIZE = 32, GARBAGE_SIZE = 8 };
	line.reserve(DATA_SIZE);
	garbage.reserve(GARBAGE_SIZE);

	bool bSuccess = false;


	// { 를 버립니다.
	std::getline(fs, garbage);

	while (true)
	{
		std::getline(fs, line);

		// 한 덩어리의 파싱이 끝났습니다.
		enum { EQUALS = 0 };
		if (line.compare("}") == EQUALS)
		{
			break;
		}
		// : 를 기준으로 문자열을 분리합니다.
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
			checkf(false, TEXT("포멧 데이터가 잘못되었습니다. : %hs"), statName);
			bSuccess = false;
			goto ERROR_END_PARSE_WEAPONS;
		}
		line.clear();
	}
	bSuccess = true;

	ERROR_END_PARSE_WEAPONS:
	return bSuccess;
}

bool USRStatistics::parseGames(std::ifstream& fs)
{
	std::string line;
	std::string garbage;
	enum { DATA_SIZE = 64, GARBAGE_SIZE = 8 };
	line.reserve(DATA_SIZE);
	garbage.reserve(GARBAGE_SIZE);

	bool bSuccess = false;
	// { 를 버립니다.
	std::getline(fs, garbage);

	while (true)
	{
		std::getline(fs, line);

		// 한 덩어리의 파싱이 끝났습니다.
		enum { EQUALS = 0 };
		if (line.compare("}") == EQUALS)
		{
			break;
		}

		// : 를 기준으로 문자열을 분리합니다.
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
			checkf(false, TEXT("포멧 데이터가 잘못되었습니다. : %hs"), statName);
			bSuccess = false;
			goto ERROR_END_PARSE_GAMES;
		}

		line.clear();
	}
	bSuccess = true;

	ERROR_END_PARSE_GAMES:
	return bSuccess;
}

bool USRStatistics::parseScore(std::ifstream& fs)
{
	std::string line;
	std::string garbage;
	enum { DATA_SIZE = 64, GARBAGE_SIZE = 8 };
	line.reserve(DATA_SIZE);
	garbage.reserve(GARBAGE_SIZE);

	bool bSuccess = false;

	// { 를 버립니다.
	std::getline(fs, garbage);

	while (true)
	{
		std::getline(fs, line);


		// 한 덩어리의 파싱이 끝났습니다.
		enum { EQUALS = 0 };
		if (line.compare("}") == EQUALS)
		{
			break;
		}

		// : 를 기준으로 문자열을 분리합니다.
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
			checkf(false, TEXT("포멧 데이터가 잘못되었습니다. : %hs"), statName);
			bSuccess = false;
			goto ERROR_END_PARSE_SCORES;
		}
		line.clear();
	}
	bSuccess = true;

	ERROR_END_PARSE_SCORES:
	return bSuccess;
}
