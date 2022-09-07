// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "Animation/AnimInstance.h"
#include "SRAnimInstance.generated.h"

/*
 *  플레이어 캐릭터 애님인스턴스.
 *	블루프린트에서 사용할 ADS, IK 값을 계산합니다.
 *	무기의 반동(sway, recoil)도 계산됩니다.
 */

class UCurveVector;
class ASRPlayerCharacter;

UCLASS()
class VERSION_API USRAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	USRAnimInstance();

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void UpdateSocketInfo();

	void SetAiming(bool IsAiming);

	void SetRecoil(bool isFiring);

	UFUNCTION(BlueprintCallable)
	void Fire();

	void RecoilStart();

	UFUNCTION(BlueprintCallable)
	void RecoilTick(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void RecoilStop();

	void Reload();

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

	UPROPERTY(BlueprintReadOnly)
	ASRPlayerCharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadOnly)
	FTransform RelativeHandTransform;

	FTransform FinalHandTransform;

	UPROPERTY(BlueprintReadOnly)
	FTransform SightTransform;

	UPROPERTY(BlueprintReadOnly)
	FTransform LeftHandTransform;

	UPROPERTY(BlueprintReadOnly)
	float AimAlpha;

	UPROPERTY(BlueprintReadOnly)
	float ReloadAlpha;

	// sway
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCurveVector* VectorCurve;

	UPROPERTY(BlueprintReadOnly)
	FVector SwayLocation;

	// turn sway
	UPROPERTY(BlueprintReadOnly)
	FRotator TurnRotation;
	FRotator OldRotation;

	UPROPERTY(BlueprintReadOnly)
	FVector TurnLocation;


	UPROPERTY(BlueprintReadOnly)
	FTransform RecoilTransform;

	FTransform FinalRecoilTransform;

	bool bInterpAiming;
	bool bInterpRelativeHand;
	bool bIsAiming;


private:

	UPROPERTY()
	class ASRPlayerController* mPlayerController;

	// weapon recoil
	FRandomStream mRandomStream;

	float mRecoveryTime;
	//float mRecoverySpeed = 10.0f;
	float mSumRecoil;

	float mFireRate;

	// recoil
	bool mbRecoil;

	bool mbFiring;

	bool mbRecoilRecovery;

	//	UPROPERTY()
//	FRotator OldCameraRotation;
	//Control rotation at the start of the recoil
	FRotator mRecoilStartRotator;

	//Control rotation change due to recoil
	FRotator mRecoilDeltaRotator;

	//Control rotation chnage due to player moving the mouse
	FRotator mPlayerDeltaRotator;

	//Temporary variable used in tick
	FRotator mDeltaRotator;

	//Timer Handles
	UPROPERTY()
	FTimerHandle mFireTimer;

	UPROPERTY()
	FTimerHandle mRecoveryTimer;
};
