// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "GameFramework/Actor.h"
#include "SRSpawnPoint.generated.h"

UENUM(BlueprintType)
enum class ESpawnPointType : uint8
{
	CharacterType,
	PlateType,
};

UENUM(BlueprintType)
enum class EMovableAxis : uint8
{
	X,
	Y
};


/*
 * 맵에 배치되는 스폰 포인트 클래스입니다.
 * 타겟 매니저가 타겟을 스폰 시킬 때 해당 클래스 개체의 정보를 사용합니다.
 * 타겟 유형, 이동할 수 있는 축(X, Y)
 */

UCLASS()
class VERSION_API ASRSpawnPoint final : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASRSpawnPoint();

	bool IsActive() const;

	void Active();

	UFUNCTION()
	void DeActive();

	bool IsCrouchable() const;

	ESpawnPointType GetSpawnPointType() const;

	EMovableAxis GetMovableAxis() const;

	UFUNCTION(BlueprintCallable)
	FVector GetSpawnLocation() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESpawnPointType mSpawnerType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMovableAxis mMovableAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool mbCrouchable;

private:

	UPROPERTY()
	UStaticMeshComponent* mPointMesh;

	bool mbActive;
};
