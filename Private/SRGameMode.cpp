// Fill out your copyright notice in the Description page of Project Settings.
#include "SRGameMode.h"
#include "SRPlayerCharacter.h"
#include "SRPlayerController.h"
#include "SRPlayerState.h"
#include "SRStatistics.h"
#include "SRTargetManager.h"
#include "SRWeapon.h"
#include "UIHUDWidget.h"
#include "UISelectModesWidget.h"
#include "Blueprint/UserWidget.h"

ASRGameMode::ASRGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/BP_PlayerCharacter.BP_PlayerCharacter_C"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerControllerClass = ASRPlayerController::StaticClass();
	PlayerStateClass = ASRPlayerState::StaticClass();

	mGameModeType = eGameModeType::None;

	enum{TIME_TO_READY = 5, TIME_GAMEPLAY = 90};
	mReadyTime = TIME_TO_READY;
	mGameTime = TIME_GAMEPLAY;
}

/*
 *  Select위젯에서 선택한 정보들을 통해서
 *	각 클래스들을 세팅하고, 게임을 시작합니다.
 *	준비시간 카운트가 시작됩니다.
 */
void ASRGameMode::SettingGameAndStartGame()
{
	auto* const playerCharacter = Cast<ASRPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	playerCharacter->InitDataFromGameMode(mModeData);

	mPlayerState->Initialize(mModeData.weapon, mModeData.game, mGameModeType, mHUD);

	mPlayerController->LoadMouseSensitivitySetting();
	mPlayerController->InitCharacterMouseAndAimingSetting(mModeData.scope);
	mPlayerController->StartGame();

	GetWorld()->GetTimerManager().SetTimer(mTimer, this, &ASRGameMode::countdownReadyGameTimer, 1.0f, true, 0.0f);
}

void ASRGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	mPlayerController = Cast<ASRPlayerController>(NewPlayer);
}

void ASRGameMode::BeginPlay()
{
	Super::BeginPlay();
	checkf(mGameModeType != eGameModeType::None, TEXT("ASRGameMode : BeginPlay - eGameModeType None 타입이 될 수 없습니다."));

	mTargetManager = GetWorld()->SpawnActor<ASRTargetManager>(ASRTargetManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);

	mPlayerController->CreateUIWidgets();
	mPlayerController->GetSelectWidget()->AddToViewport();

	mPlayerState = Cast<ASRPlayerState>(mPlayerController->PlayerState);
	mTargetManager->SetMovableTargetMode(mGameModeType);

	mHUD = mPlayerController->GetHUDWidget();
}

void ASRGameMode::StartPlay()
{
	Super::StartPlay();

	USRStatistics::Initialize();
}

void ASRGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	mTargetManager->Destroy();
	GetWorld()->GetTimerManager().ClearTimer(mTimer);
}

eScopeType ASRGameMode::GetScopeType() const
{
	return mModeData.scope;
}

UUIHUDWidget* ASRGameMode::GetHUDWidget() const
{
	return mHUD;
}

ASRPlayerState* ASRGameMode::GetPlayerState() const
{
	return mPlayerState;
}

ASRTargetManager* ASRGameMode::GetTargetManager() const
{
	return mTargetManager;
}

void ASRGameMode::SetDebugMode(bool isActive)
{
	mbDebugMode = isActive;
	ASRWeapon::bIsDebugMode = isActive;
}

bool ASRGameMode::IsDebugMode() const
{
	return mbDebugMode;
}

void ASRGameMode::countdownReadyGameTimer()
{
	--mReadyTime;
	mHUD->UpdateRemainingTime(mReadyTime);

	if(mReadyTime <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(mTimer);
		startMainGame();
	}
}

void ASRGameMode::countdownMainGameTimer()
{
	--mGameTime;
	mHUD->UpdateRemainingTime(mGameTime);
	if (mGameTime <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(mTimer);
		endMainGame();
	}
}

void ASRGameMode::startMainGame()
{
	GetWorld()->GetTimerManager().SetTimer(mTimer, this, &ASRGameMode::countdownMainGameTimer, 1.0f, true, 0.0f);
	mTargetManager->RandomTargetSpawn();
	mTargetManager->RandomTargetSpawn();
	mPlayerState->SetRecordMode(true);
}

void ASRGameMode::endMainGame()
{
	// 한 게임이 끝나면 새로운 스텟을 갱신합니다.
	mPlayerState->UpdateToStatistics();
	mPlayerController->EndGame();
}

/*
 * Select 위젯에서 선택한 정보를 받아와 저장하고
 * 타겟 메니저를 세팅합니다.(타겟의 타입)
 */
void ASRGameMode::SetGameModeData(FGameModeData& modeData, bool isCharacterType)
{
	mModeData = modeData;
	// setter니까 set만 하고 startgame setting으로 이동.
	mTargetManager->SetTargetType(isCharacterType);

	FString gameModeString;
	switch(mModeData.game)
	{
		case eGameType::Battlefield:
			gameModeString = TEXT("Battlefield Mode");
			break;
		case eGameType::RainbowSix:
			gameModeString = TEXT("RainbowSix Mode");
			break;
		case eGameType::Tarkov:
			gameModeString = TEXT("EscapeFromTarkov Mode");
			break;
	}

	mHUD->UpdateGameMode(gameModeString);
}

void ASRGameMode::PauseGame()
{
	GetWorld()->GetTimerManager().PauseTimer(mTimer);
}

void ASRGameMode::ResumeGame()
{
	GetWorld()->GetTimerManager().UnPauseTimer(mTimer);
}
