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
 * Ÿ�� �Ŵ��� Ŭ�����Դϴ�.
 * ���� ����Ʈ�� ���Ӹ�� Ŭ������ ������ ���ؼ�
 * Ÿ���� �����ϰ� �����մϴ�.
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
