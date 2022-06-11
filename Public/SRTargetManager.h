// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Version.h"
#include <random>
#include "GameFramework/Actor.h"
#include "SRTargetManager.generated.h"

enum class EGameModeType : uint8;
class ASRSpawnPoint;


/*
 * Ÿ�� �Ŵ��� Ŭ�����Դϴ�.
 * ���� ����Ʈ�� ���Ӹ�� Ŭ������ ������ ���ؼ�
 * Ÿ���� �����ϰ� �����մϴ�.
 */

UCLASS()
class VERSION_API ASRTargetManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASRTargetManager();


	UFUNCTION()
	void RandomTargetSpawn();

	UFUNCTION()
	void SetMovableTargetMode(EGameModeType mode);

	UFUNCTION()
	void SetTargetType(bool isCharacter);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TArray<AActor*> mSpawnPointList;

	UPROPERTY()
	TArray<ASRSpawnPoint*> mMovableSpawnPointList;

	UPROPERTY()
	TArray<ASRSpawnPoint*> mPlateSpawnPointList;

	UPROPERTY()
	TArray<ASRSpawnPoint*> mCharacterSpawnPointList;

	UPROPERTY()
	TArray<class ASRTargetCharacter*> mSpawnedTargetList;

	EGameModeType mGameModeType;

	bool mbMovableTargetMode;
	bool mbIsCharacterType;

	std::random_device mRandomDevice;
	std::mt19937 mGen;

};
