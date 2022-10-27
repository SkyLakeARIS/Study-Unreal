// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Version.h"
#include "GameModeData.h"
#include "GameFramework/Character.h"
#include "SRPlayerCharacter.generated.h"


DECLARE_DELEGATE_OneParam(FOnCrossHairVisibility, ESlateVisibility);


enum eBehaviorStateFlags : uint8
{
	FIRING = 0x01,
	RELOADING = 0x02,
	FULL_RELOADING = 0x04,
	AIMING = 0x08,
	BOLTACTION = 0x10,
	EMPTY_MAG = 0x20,
	WAIT_TO_END_ANIMATION = 0x40,
};
const uint8 CLEAR_STATE = 0x00;


class ASRPlayerController;
class ASRWeapon;
class USRAnimInstance;
enum class EHitType : uint8;

/*
 * 플레이어 캐릭터 클래스입니다.
 */

UCLASS(config=Game)
class ASRPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASRPlayerCharacter();

	/*
	 *  Setter
	 */
	UFUNCTION()
	void InitDataFromGameMode(const FGameModeData modeData);

	void SetAimingType(const EAimingType newSetting);

	inline void SetBehaviorFlag(const uint8 flag);
	inline void UnsetBehaviorFlag(const uint8 flag);


	void SetMouseSensitivity(const float hipYaw, const float hipPitch, const float aimYaw, const float aimPitch);

	/*
	 * getter
	 */ 
	UFUNCTION(BlueprintPure)
	USkeletalMeshComponent* GetMesh1P() const { return mMesh1P; }
	UFUNCTION(BlueprintPure)
	UCameraComponent* GetFirstPersonCameraComponent() const { return mFirstPersonCameraComponent; }
	UFUNCTION(BlueprintPure)
	EWeaponType GetWeaponType() const;
	UFUNCTION(BlueprintPure)
	EScopeType GetScopeType() const;
	UFUNCTION(BlueprintPure)
	EGameType GetGameType() const;
	UFUNCTION(BlueprintPure)
	ASRWeapon* GetWeapon() const;
	UFUNCTION(BlueprintPure)
	EAimingType GetAimingType() const;
	UFUNCTION(BlueprintPure)
	int32 GetBehaviorFlag() const;
	UFUNCTION(BlueprintCallable)
	USRAnimInstance* GetAnimInstance() const;

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

	void StartFire();
	void StopFire();

	void Reload();

	void SwitchFireMode();

	void SetAim();
	void SetHip();

	void MoveForward(float Val);
	void MoveRight(float Val);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Character)
	USkeletalMeshComponent* mMesh1P;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	UCameraComponent* mFirstPersonCameraComponent;

	UPROPERTY(BlueprintReadOnly)
	USRAnimInstance* mAnimInstance;

	UPROPERTY()
	ASRPlayerController* mPlayerController;

	FOnCrossHairVisibility mOnCrossHairVisibility;

	UPROPERTY()
	ASRWeapon* mWeapon;

private:
	/*
	 *  behavior state
	 */
	int32 mBehaviorFlag;

	/*
	 *  mouse setting
	 */
	float mHipYawSensitivity;
	float mHipPitchSensitivity;

	float mAimYawSensitivity;
	float mAimPitchSensitivity;

	EAimingType mAimingType;

	/*
	 *  game setting
	 */
	EGameType mGameType;
	EScopeType mScopeType;
	EWeaponType mWeaponType;
};

void ASRPlayerCharacter::SetBehaviorFlag(const uint8 flag)
{
	mBehaviorFlag |= flag;
}

void ASRPlayerCharacter::UnsetBehaviorFlag(const uint8 flag)
{
	mBehaviorFlag &= ~flag;
}
