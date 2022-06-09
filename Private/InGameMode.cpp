// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMode.h"
#include "CharacterPlayerController.h"
#include "SRPlayerState.h"
#include "SRTargetManager.h"

AInGameMode::AInGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerControllerClass = ACharacterPlayerController::StaticClass();
	PlayerStateClass = ASRPlayerState::StaticClass();

	mGameModeType = EGameModeType::Static_ShortRange;
}

void AInGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AInGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	playerController = Cast<ACharacterPlayerController>(NewPlayer);
	FTimerHandle initTarget;
	GetWorld()->GetTimerManager().SetTimer(initTarget, this, &AInGameMode::initTargetManagerMode, 0.5f, false);
}

void AInGameMode::initTargetManagerMode()
{
	playerController->GetTargetManager()->SetMovableTargetMode(mGameModeType);
}
