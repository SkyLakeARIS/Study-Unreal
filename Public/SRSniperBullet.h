// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "SRProjectile.h"
#include "SRSniperBullet.generated.h"

/*
 * �������� �����ϸ� ���˴ϴ�.
 * �ӵ��� ���� ���� ź�Դϴ�.
 */

UCLASS()
class VERSION_API ASRSniperBullet final : public ASRProjectile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASRSniperBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
