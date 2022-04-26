// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMode.h"

#include "CharacterPlayerController.h"

AInGameMode::AInGameMode()
	{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
		DefaultPawnClass = PlayerPawnClassFinder.Class;
		PlayerControllerClass = ACharacterPlayerController::StaticClass();
		PlayerStateClass = ASRPlayerState::StaticClass();
	}

void AInGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AInGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}
