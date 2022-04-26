// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Version.h"
#include "Animation/AnimInstance.h"
#include "ADAnimInstance.generated.h"

/**
 * 
 */

class UCurveVector;
class AVersionCharacter;

UCLASS()
class VERSION_API UADAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UADAnimInstance();

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;


	UPROPERTY(BlueprintReadOnly, Category = TUTO)
	AVersionCharacter* Character;

	UPROPERTY(BlueprintReadOnly, Category = TUTO)
	FTransform RelativeHandTransform;

	FTransform FinalHandTransform;

	UPROPERTY(BlueprintReadOnly, Category = TUTO)
	FTransform SightTransform;

	UPROPERTY(BlueprintReadOnly, Category = TUTO)
	FTransform LeftHandTransform;

	UPROPERTY(BlueprintReadOnly, Category = TUTO)
	float AimAlpha;

	UPROPERTY(BlueprintReadOnly, Category = TUTO)
	float ReloadAlpha;

	// sway
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TUTO)
	UCurveVector* VectorCurve;

	UPROPERTY(BlueprintReadOnly, Category = TUTO)
	FVector SwayLocation;

	// turn sway
	UPROPERTY(BlueprintReadOnly, Category = TUTO)
	FRotator TurnRotation;
	FRotator OldRotation;

	UPROPERTY(BlueprintReadOnly, Category = TUTO)
	FVector TurnLocation;


	// weapon recoil
	UPROPERTY(BlueprintReadOnly, Category = TUTO)
	FTransform RecoilTransform;
	FTransform FinalRecoilTransform;

	bool bInterpAiming;
	bool bInterpRelativeHand;
	bool bIsAiming;
protected:
	void SetSightTransform();
	void SetRelativeHandTransform();
	void SetFinalHandTransform();
	void SetLeftHandIK();

	void InterpAiming(float DeltaSeconds);
	void InterpRelativeHand(float DeltaSeconds);

	void MoveVectorCurve(float DeltaSeconds);
	void RotateWithRotation(float DeltaSeconds);

	void InterpFinalRecoil(float DeltaSeconds);
	void InterpRecoil(float DeltaSeconds);

public:
	void SetAiming(bool IsAiming);

	void CycledOptic();

	void Reload();

	UFUNCTION(BlueprintCallable, Category=TUTO)
	void StopReload();

	UFUNCTION(BlueprintCallable, Category = TUTO)
	void Fire();
};
