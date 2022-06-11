// Fill out your copyright notice in the Description page of Project Settings.


#include "SRMovableLongRangeGameMode.h"
#include "SRPlayerController.h"
#include "SRPlayerState.h"
#include "SRTargetManager.h"

ASRMovableLongRangeGameMode::ASRMovableLongRangeGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/BP_PlayerCharacter.BP_PlayerCharacter_C"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerControllerClass = ASRPlayerController::StaticClass();
	PlayerStateClass = ASRPlayerState::StaticClass();

	mGameModeType = EGameModeType::Movable_LongRange;
}

void ASRMovableLongRangeGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASRMovableLongRangeGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	playerController = Cast<ASRPlayerController>(NewPlayer);
	FTimerHandle initTarget;
	GetWorld()->GetTimerManager().SetTimer(initTarget, this, &ASRMovableLongRangeGameMode::initTargetManagerMode, 0.5f, false);
}

void ASRMovableLongRangeGameMode::initTargetManagerMode()
{
	playerController->GetTargetManager()->SetMovableTargetMode(mGameModeType);
}
