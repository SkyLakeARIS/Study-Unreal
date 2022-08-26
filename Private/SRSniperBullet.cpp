// Fill out your copyright notice in the Description page of Project Settings.


#include "SRSniperBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ASRSniperBullet::ASRSniperBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 250000.f;
	ProjectileMovement->MaxSpeed = 500000.f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
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

