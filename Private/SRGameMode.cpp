// Fill out your copyright notice in the Description page of Project Settings.
#include "SRGameMode.h"
#include "SRPlayerController.h"
#include "SRPlayerState.h"
#include "SRTargetManager.h"

ASRGameMode::ASRGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/BP_PlayerCharacter.BP_PlayerCharacter_C"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerControllerClass = ASRPlayerController::StaticClass();
	PlayerStateClass = ASRPlayerState::StaticClass();

	mGameModeType = EGameModeType::None;
}

void ASRGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASRGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	FTimerHandle initTarget;
	GetWorld()->GetTimerManager().SetTimer(initTarget, this, &ASRGameMode::initTargetManagerMode, 0.5f, false);
}

void ASRGameMode::initTargetManagerMode() const
{
	checkf(mGameModeType != EGameModeType::None, TEXT("ASRGameMode : initTargetManagerMode - EGameModeType None 타입이 될 수 없습니다."));

	const auto playerController = Cast<ASRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	playerController->GetTargetManager()->SetMovableTargetMode(mGameModeType);
}
