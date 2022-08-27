// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "SRProjectile.h"
#include "SRRifleBullet.generated.h"

/*
 * 소총을 선택하면 사용됩니다.
 * 적당한 속도의 탄입니다.
 */
UCLASS()
class VERSION_API ASRRifleBullet : public ASRProjectile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASRRifleBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
