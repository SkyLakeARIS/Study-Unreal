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

UCLASS()
class VERSION_API ASRSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASRSpawnPoint();

	UFUNCTION(BlueprintCallable)
	FVector GetSpawnLocation();

	bool IsActive() const;

	void Active();

	UFUNCTION()
	void DeActive();

	bool IsCrouchable();

	ESpawnPointType GetSpawnPointType();

	FVector GetMovableEndLocation();

	EMovableAxis GetMovableAxis();

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

	FVector mMoveableEndLocation;
};
