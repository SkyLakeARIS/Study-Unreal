// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Version.h"
#include "GameModeData.h"
#include "GameFramework/GameModeBase.h"
#include "SRMovableMidRangeGameMode.generated.h"

/**
 * 
 */
UCLASS()
class VERSION_API ASRMovableMidRangeGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ASRMovableMidRangeGameMode();

	virtual void PostInitializeComponents() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

private:

	void initTargetManagerMode();

private:
	EGameModeType mGameModeType;

	UPROPERTY()
	class ACharacterPlayerController* playerController;

};
