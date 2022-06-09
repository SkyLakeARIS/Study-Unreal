// Fill out your copyright notice in the Description page of Project Settings.


#include "SRMidRangeGameMode.h"
#include "CharacterPlayerController.h"
#include "SRPlayerState.h"
#include "SRTargetManager.h"

ASRMidRangeGameMode::ASRMidRangeGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerControllerClass = ACharacterPlayerController::StaticClass();
	PlayerStateClass = ASRPlayerState::StaticClass();

	mGameModeType = EGameModeType::Static_MidRange;
}

void ASRMidRangeGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASRMidRangeGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	playerController = Cast<ACharacterPlayerController>(NewPlayer);
	FTimerHandle initTarget;
	GetWorld()->GetTimerManager().SetTimer(initTarget, this, &ASRMidRangeGameMode::initTargetManagerMode, 0.5f, false);
}

void ASRMidRangeGameMode::initTargetManagerMode()
{
	playerController->GetTargetManager()->SetMovableTargetMode(mGameModeType);
}
