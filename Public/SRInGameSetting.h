// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "GameModeData.h"
#include "GameFramework/SaveGame.h"
#include "SRInGameSetting.generated.h"

/*
 * �÷��̾��� �ΰ��� ������ �����ϴ� Ŭ�����Դϴ�.
 * ���� ���(hold, toggle), ���ذ溰 ���콺 ������ �����մϴ�.
 * �����̸��� ��ȣ�� �����Ǿ� �ֽ��ϴ�.
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
