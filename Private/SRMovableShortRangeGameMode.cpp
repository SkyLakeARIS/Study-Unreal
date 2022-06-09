// Fill out your copyright notice in the Description page of Project Settings.


#include "SRMovableShortRangeGameMode.h"
#include "CharacterPlayerController.h"
#include "SRPlayerState.h"
#include "SRTargetManager.h"

ASRMovableShortRangeGameMode::ASRMovableShortRangeGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerControllerClass = ACharacterPlayerController::StaticClass();
	PlayerStateClass = ASRPlayerState::StaticClass();

	mGameModeType = EGameModeType::Movable_ShortRange;
}

void ASRMovableShortRangeGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASRMovableShortRangeGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	playerController = Cast<ACharacterPlayerController>(NewPlayer);
	FTimerHandle initTarget;
	GetWorld()->GetTimerManager().SetTimer(initTarget, this, &ASRMovableShortRangeGameMode::initTargetManagerMode, 0.5f, false);
}

void ASRMovableShortRangeGameMode::initTargetManagerMode()
{
	playerController->GetTargetManager()->SetMovableTargetMode(mGameModeType);
}
