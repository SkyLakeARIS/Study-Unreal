// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Version.h"
#include <random>
#include "GameFramework/Actor.h"
#include "SRTargetManager.generated.h"

enum class EGameModeType : uint8;
class ASRSpawnPoint;


/*
 * 타겟 매니저 클래스입니다.
 * 스폰 포인트와 게임모드 클래스의 정보를 통해서
 * 타겟을 적절하게 생성합니다.
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
