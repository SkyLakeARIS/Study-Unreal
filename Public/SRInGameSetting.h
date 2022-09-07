// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "GameModeData.h"
#include "GameFramework/SaveGame.h"
#include "SRInGameSetting.generated.h"

/*
 * 플레이어의 인게임 설정을 저장하는 클래스입니다.
 * 조준 방식(hold, toggle), 조준경별 마우스 감도를 저장합니다.
 * 슬롯이름과 번호는 고정되어 있습니다.
 */
UCLASS()
class VERSION_API USRInGameSetting : public USaveGame
{
	GENERATED_BODY()
public:
	USRInGameSetting();

	UFUNCTION()
	FString GetSlotName() const;

	UFUNCTION()
	int32 GetSlotIndex() const;

	UPROPERTY()
	FMouseSensitivity MouseSensitivity;

	UPROPERTY()
	EAimingType AimingType;

private:
	FString mSlotName;

	int32 mSlotIndex;
};
