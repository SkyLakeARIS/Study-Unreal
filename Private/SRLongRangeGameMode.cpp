// Fill out your copyright notice in the Description page of Project Settings.


#include "SRLongRangeGameMode.h"

#include "CharacterPlayerController.h"
#include "SRPlayerState.h"
#include "SRTargetManager.h"

ASRLongRangeGameMode::ASRLongRangeGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerControllerClass = ACharacterPlayerController::StaticClass();
	PlayerStateClass = ASRPlayerState::StaticClass();

	mGameModeType = EGameModeType::Static_LongRange;
}

void ASRLongRangeGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASRLongRangeGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	playerController = Cast<ACharacterPlayerController>(NewPlayer);
	FTimerHandle initTarget;
	GetWorld()->GetTimerManager().SetTimer(initTarget, this, &ASRLongRangeGameMode::initTargetManagerMode, 0.5f, false);
}

void ASRLongRangeGameMode::initTargetManagerMode()
{
	playerController->GetTargetManager()->SetMovableTargetMode(mGameModeType);
}
