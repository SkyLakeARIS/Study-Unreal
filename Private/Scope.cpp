// Fill out your copyright notice in the Description page of Project Settings.


#include "Scope.h"

#include "Components/SceneCaptureComponent2D.h"

// Sets default values
AScope::AScope()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = mSceneComponent;
	mMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Scope"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SCOPE(TEXT("/Game/Assets/FPS_Weapon_Bundle/Weapons/Meshes/Accessories/SM_Scope_25x56_X.SM_Scope_25x56_X"));
	if(SM_SCOPE.Succeeded())
	{
		mMesh->SetStaticMesh(SM_SCOPE.Object);
	}
	mSceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
}

// Called when the game starts or when spawned
void AScope::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AScope::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UStaticMesh* AScope::GetStaticMesh() const
{
	return mMesh->GetStaticMesh();
}

