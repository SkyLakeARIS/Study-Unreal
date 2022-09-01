// Fill out your copyright notice in the Description page of Project Settings.
#include "SRHandGunBullet.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ASRHandGunBullet::ASRHandGunBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 55000.f;
	ProjectileMovement->MaxSpeed = 360000.f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
}

// Called when the game starts or when spawned
void ASRHandGunBullet::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASRHandGunBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

