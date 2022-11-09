// Fill out your copyright notice in the Description page of Project Settings.
#include "SRRifleBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ASRRifleBullet::ASRRifleBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mProjectileMovement->UpdatedComponent = mCollisionComp;
	mProjectileMovement->InitialSpeed = 70000.f;
	mProjectileMovement->MaxSpeed = 100000.f;
	mProjectileMovement->bRotationFollowsVelocity = false;
	mProjectileMovement->bShouldBounce = false;
}

// Called when the game starts or when spawned
void ASRRifleBullet::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASRRifleBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

