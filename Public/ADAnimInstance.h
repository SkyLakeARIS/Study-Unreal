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

	UFUNCTION()
	void UpdateSocketInfo();

	void SetAiming(bool IsAiming);

	void SetFiring(bool isFiring);

	UFUNCTION(BlueprintCallable, Category = TUTO)
	void Fire();

	void RecoilStart();
	//Needs to be called on event tick to update the control rotation.
	UFUNCTION(BlueprintCallable)
	void RecoilTick(float DeltaTime);

	//Called when firing stops
	UFUNCTION(BlueprintCallable)
	void RecoilStop();

	void Reload();

	UFUNCTION(BlueprintCallable, Category = TUTO)
	void StopReload();
	UFUNCTION(BlueprintPure, Category = TUTO)
	AVersionCharacter* GetCharacter1() const { return PlayerCharacter; }
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

protected:


	//Automatically called in RecoilStart(), no need to call this explicitly
	UFUNCTION()
	void RecoilTimerFunction();


	//This function is automatically called, no need to call this. It is inside the Tick function
	UFUNCTION()
	void RecoveryStart();

	//This function too is automatically called from the recovery start function.
	UFUNCTION()
	void RecoveryTimerFunction();



public:

	UPROPERTY(BlueprintReadOnly, Category = TUTO)
	AVersionCharacter* PlayerCharacter;

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

	// recoil pattern
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TUTO)
		UCurveVector* RecoilCurve;

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

	// recoil
	UPROPERTY(BlueprintReadWrite)
		bool bRecoil;
	UPROPERTY(BlueprintReadWrite)
		bool bFiring;
	UPROPERTY(BlueprintReadWrite)
		bool bRecoilRecovery;

	//Timer Handles
	UPROPERTY(BlueprintReadwrite)
		FTimerHandle FireTimer;
	UPROPERTY(BlueprintReadwrite)
		FTimerHandle RecoveryTimer;

	/*Optional variables to customize how fast the recoil resets and what is the max time
		upto which the recovery can last */

	UPROPERTY(BlueprintReadWrite)
		float RecoveryTime = 3.0f;
	UPROPERTY(BlueprintReadWrite)
		float RecoverySpeed = 10.0f;
	float sumRecoil = 0.0f;

	UPROPERTY()
		FRotator oldRotation;
	//Control rotation at the start of the recoil
	UPROPERTY()
		FRotator RecoilStartRot;
	//Control rotation change due to recoil
	UPROPERTY()
		FRotator RecoilDeltaRot;
	//Control rotation chnage due to player moving the mouse
	UPROPERTY()
		FRotator PlayerDeltaRot;
	//Temporary variable used in tick
	UPROPERTY(BlueprintReadWrite)
		FRotator Del;

	//Player controller reference
	UPROPERTY(BlueprintReadWrite)
	class ACharacterPlayerController* mPlayerController;

	UPROPERTY(BlueprintReadWrite)
		float FireRate;
};
