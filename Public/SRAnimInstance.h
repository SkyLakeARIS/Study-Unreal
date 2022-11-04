// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "Animation/AnimInstance.h"
#include "SRAnimInstance.generated.h"

enum class eGameType : uint8;
class UCurveVector;
class ASRPlayerCharacter;

/*
 *  플레이어 캐릭터 애님인스턴스.
 *	블루프린트에서 사용할 ADS, IK 값을 계산합니다.
 *	무기의 반동(sway, recoil)도 계산됩니다.
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

	bool bInterpAiming;				 // 조준 중/해제 애니메이션 재생중에 보간이 완료되었는지를 나타내는 변수입니다.( false: 보간완료)
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
