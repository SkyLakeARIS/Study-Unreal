// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "Animation/AnimInstance.h"
#include "SRAnimInstance.generated.h"

enum class eGameType : uint8;
class UCurveVector;
class ASRPlayerCharacter;

/*
 *  �÷��̾� ĳ���� �ִ��ν��Ͻ�.
 *	�������Ʈ���� ����� ADS, IK ���� ����մϴ�.
 *	������ �ݵ�(sway, recoil)�� ���˴ϴ�.
 */
UCLASS()
class VERSION_API USRAnimInstance final : public UAnimInstance
{
	GENERATED_BODY()

public:
	USRAnimInstance();

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void SetAim();
	UFUNCTION(BlueprintCallable)
	void UnsetAim();

	void SetFire();
	UFUNCTION(BlueprintCallable)
	void UnsetFire();

	UFUNCTION()
	void UpdateSocketInfo();

	UFUNCTION(BlueprintCallable)
	void Fire();

	void RecoilStart();

	UFUNCTION(BlueprintCallable)
	void RecoilTick(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void RecoilStop();

	//void Reload();

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
	void recoilTimerFunction();

	//This function is automatically called, no need to call this. It is inside the Tick function
	UFUNCTION()
	void recoveryStart();

	//This function too is automatically called from the recovery start function.
	UFUNCTION()
	void recoveryTimerFunction();

private:

	void calcRecoilFactor(eGameType gameType);

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

	bool bInterpAiming;				 // ���� ��/���� �ִϸ��̼� ����߿� ������ �Ϸ�Ǿ������� ��Ÿ���� �����Դϴ�.( false: �����Ϸ�)
	bool bInterpRelativeHand;

private:

	UPROPERTY()
	class ASRPlayerController* mPlayerController;

	float mAimingAlpha;

	FRandomStream mRandomStream;

	float mRecoveryTime;
	float mSumRecoil;
	float mSumHorizonRecoil;

	float mFireRate;

	// recoil
	bool mbRecoil;

	bool mbFiring;

	bool mbRecoilRecovery;


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
