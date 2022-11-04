// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include <random>
#include "GameFramework/Actor.h"
#include "SRTargetManager.generated.h"

class ASRTargetCharacter;
enum class eGameModeType : uint8;
class ASRSpawnPoint;


/*
 * 타겟 매니저 클래스입니다.
 * 스폰 포인트와 게임모드 클래스의 정보를 통해서
 * 타겟을 적절하게 생성합니다.
 */

UCLASS()
class VERSION_API ASRTargetManager final : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASRTargetManager();

	UFUNCTION()
	void RandomTargetSpawn();

	void RemoveTargetFromTargetList(FVector position);

	/*
	 *  setter
	 */
	UFUNCTION()
	void SetMovableTargetMode(eGameModeType mode);
	UFUNCTION()
	void SetTargetType(bool isCharacter);

	/*
	 *  getter
	 */
	TArray<FVector, TFixedAllocator<8>> GetSpawnedTargetPositions() const;
	eGameModeType GetGameModeType() const;
	bool IsMovableTargetMode() const;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TArray<AActor*> mSpawnPointList;

	/*
	 *  target lists
	 */
	TArray<ASRSpawnPoint*, TFixedAllocator<64>> mPlateSpawnPointList;
	TArray<ASRSpawnPoint*, TFixedAllocator<64>> mCharacterSpawnPointList;

	/*
	 *  spawned target (for indicator)
	 */
	TArray<ASRTargetCharacter*, TFixedAllocator<8>> mSpawnedTargetList;

	eGameModeType mGameModeType;

	bool mbMovableTargetMode;
	bool mbIsCharacterType;

	std::random_device mRandomDevice;
	std::mt19937 mGen;
};
