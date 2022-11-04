// Fill out your copyright notice in the Description page of Project Settings.
#include "SRSniperBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ASRSniperBullet::ASRSniperBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mProjectileMovement->UpdatedComponent = mCollisionComp;
	mProjectileMovement->InitialSpeed = 90000.f;
	mProjectileMovement->MaxSpeed = 100000.f;
	mProjectileMovement->bRotationFollowsVelocity = false;
	mProjectileMovement->bShouldBounce = false;
}

// Called when the game starts or when spawned
void ASRSniperBullet::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASRSniperBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

