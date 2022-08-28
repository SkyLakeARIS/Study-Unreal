// Fill out your copyright notice in the Description page of Project Settings.
#include "SRSpawnPoint.h"

// Sets default values
ASRSpawnPoint::ASRSpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	mPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("POINTMESH"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_MESH(TEXT("/Game/Assets/Target/SM_SpawnPoint.SM_SpawnPoint"));
	if(SM_MESH.Succeeded())
	{
		mPointMesh->SetStaticMesh(SM_MESH.Object);
	}
	RootComponent = mPointMesh;
	mPointMesh->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));

	mbCrouchable = false;
	mbActive = false;
}

FVector ASRSpawnPoint::GetSpawnLocation() const
{
	return GetActorLocation();
}

bool ASRSpawnPoint::IsActive() const
{
	return mbActive;
}

// true이면 해당 위치에 타겟이 있으므로 스폰이 불가능합니다.
// false이면 타겟을 스폰할 수 있습니다.
void ASRSpawnPoint::Active()
{
	mbActive = true;
}

// Called when the game starts or when spawned
void ASRSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	mPointMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mPointMesh->SetCollisionObjectType(ECC_Visibility);
}

void ASRSpawnPoint::DeActive()
{
	mbActive = false;
}

bool ASRSpawnPoint::IsCrouchable() const
{
	return mbCrouchable;
}

ESpawnPointType ASRSpawnPoint::GetSpawnPointType() const
{
	return mSpawnerType;
}

EMovableAxis ASRSpawnPoint::GetMovableAxis() const
{
	return mMovableAxis;
}

