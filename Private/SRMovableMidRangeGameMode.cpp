// Fill out your copyright notice in the Description page of Project Settings.


#include "SRMovableMidRangeGameMode.h"

#include "CharacterPlayerController.h"
#include "SRPlayerState.h"
#include "SRTargetManager.h"

ASRMovableMidRangeGameMode::ASRMovableMidRangeGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerControllerClass = ACharacterPlayerController::StaticClass();
	PlayerStateClass = ASRPlayerState::StaticClass();

	mGameModeType = EGameModeType::Movable_MidRange;
}

void ASRMovableMidRangeGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASRMovableMidRangeGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	playerController = Cast<ACharacterPlayerController>(NewPlayer);
	FTimerHandle initTarget;
	GetWorld()->GetTimerManager().SetTimer(initTarget, this, &ASRMovableMidRangeGameMode::initTargetManagerMode, 0.5f, false);
}

void ASRMovableMidRangeGameMode::initTargetManagerMode()
{
	playerController->GetTargetManager()->SetMovableTargetMode(mGameModeType);
}
