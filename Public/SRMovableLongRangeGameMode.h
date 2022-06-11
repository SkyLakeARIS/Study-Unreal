// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Version.h"
#include "GameModeData.h"
#include "GameFramework/GameModeBase.h"
#include "SRMovableLongRangeGameMode.generated.h"

/*
 * LongRange �̵��� ���� ������ ���� ���Ӹ���Դϴ�.
 */
UCLASS()
class VERSION_API ASRMovableLongRangeGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ASRMovableLongRangeGameMode();

	virtual void PostInitializeComponents() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

private:

	void initTargetManagerMode();

private:
	EGameModeType mGameModeType;
	UPROPERTY()
	class ASRPlayerController* playerController;
};
