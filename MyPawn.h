// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include <engine/Classes/Camera/CameraComponent.h>
#include "MyPawn.generated.h"

UCLASS()
class MYSTUDYPROJECT_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float value);

	void MoveRight(float value);


	void MoveMouseX(float value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UPROPERTY(EditAnywhere);
	USceneComponent* mVisibleComponent;

	UPROPERTY(EditAnywhere);
	UCameraComponent* mCamera;

	FVector mCurrentVelocity;

	float mTheta;

};
