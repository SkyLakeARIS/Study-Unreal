// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Version.h"
#include "GameModeData.h"
#include "GameFramework/GameModeBase.h"
#include "SRGameMode.generated.h"

/**
 *  ���� ������ ���� ���Ӹ���Դϴ�.
 *	�������Ʈ�� ����Ͽ� �����մϴ�.
 */
UCLASS(BlueprintType, Blueprintable)
class VERSION_API ASRGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ASRGameMode();

	virtual void PostInitializeComponents() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;


private:

	void initTargetManagerMode();

protected:
	UPROPERTY(BlueprintReadWrite)
	EGameModeType mGameModeType;

	UPROPERTY()
	class ASRPlayerController* playerController;
};
