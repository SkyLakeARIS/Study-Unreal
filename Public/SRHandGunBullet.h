// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "SRProjectile.h"
#include "SRHandGunBullet.generated.h"

/*
 * 권총을 선택하면 사용됩니다.
 * 속도가 가장 느린 탄입니다.
 */

UCLASS()
class VERSION_API ASRHandGunBullet : public ASRProjectile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASRHandGunBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
