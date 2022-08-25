// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Version.h"
#include "GameModeData.h"
#include "GameFramework/Character.h"
#include "SRPlayerCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FOnCrossHairVisibility, ESlateVisibility);


enum class EHitType : uint8;
class USRAnimInstance;

/*
 * �÷��̾� ĳ���� Ŭ�����Դϴ�.
 */

UCLASS(config=Game)
class ASRPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASRPlayerCharacter();

	virtual void PossessedBy(AController* NewController) override;

	virtual void PostInitializeComponents() override;

	// getter
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
	USkeletalMeshComponent* GetNewWeapon()  const { return mWeapon; }
	UFUNCTION(BlueprintPure)
	UChildActorComponent* GetNewScope() const { return mScope; }
	UFUNCTION(BlueprintPure)
	EAimingType GetAimingType() const;

	UFUNCTION(BlueprintPure)
	bool IsAimimg() const;
	UFUNCTION(BlueprintPure)
	bool IsCanFire() const;
	UFUNCTION(BlueprintPure)
	bool IsNeedBoltAction() const;
	UFUNCTION(BlueprintPure)
	bool IsFiring() const;
	UFUNCTION(BlueprintPure)
	bool IsEmptyMag() const;
	UFUNCTION(BlueprintPure)
	bool IsReload() const;

	// setter
	void SetAimToggleOrHold(EAimingType newType);

	UFUNCTION(BlueprintCallable)
	void SniperMoveSocket(bool active);
	UFUNCTION(BlueprintCallable)
	void NotifyBoltActionEnd();
	UFUNCTION(BlueprintCallable)
	void NotifyReloadEnd();

	UFUNCTION()
	void InitGameMode(FGameModeData modeData);

	void SaveInGameSetting();
	void LoadInGameSetting();

protected:

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	void tryBindSelectModesUI();

	void StartFire();
	void StopFire();
	void BurstFire();
	void FireShot();

	void Reload();

	void SwitchFireMode();

	void SetAim();
	void SetHip();

	void endBehaviorDelay();
	void endWeaponDelay();

	void MoveForward(float Val);
	void MoveRight(float Val);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

public:

	FMouseSensitivity MouseSetting;	// make getter, move protected

protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Character)
	USkeletalMeshComponent* mMesh1P;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	UCameraComponent* mFirstPersonCameraComponent;

	UPROPERTY(BlueprintReadOnly)
	USRAnimInstance* mTutAnimInstance;

	UPROPERTY()
	class ASRPlayerController* mPlayerController;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ASRProjectile> mProjectileClass;

	// UI
	FOnCrossHairVisibility mOnCrossHairVisibility;

	/*
	 *  Weapon and scope
	 */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	TSubclassOf<class ASRWeapon> mWeaponDataClass;
	UPROPERTY()
	ASRWeapon* mWeaponData;

	UPROPERTY()
	USkeletalMeshComponent* mWeapon;
	UPROPERTY()
	UChildActorComponent* mScope;

	FString mScopeLocationSocketName;
	FString mWeaponLocationSocketName;

	// sounds and effect
	UPROPERTY()
	USoundBase* mFireSound;
	UPROPERTY()
	USoundBase* mSwitchFireModeSound;
	UPROPERTY()
	USoundBase* mDryFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Effect")
	class UParticleSystem* mMuzzleParticles;

	// game mode data
	FGameModeData mGameModeData;

	EAimingType mAimingType;


	// character state
	FTimerHandle mBehaviorDelayTimer;
	const short CAN_BEHAVIOR = 1;
	const short CAN_NOT_BEHAVIOR = 0;
	short mBehaviorFlag;				// idle�� ������ � �ൿ�� �ϰ� ������ �ٸ� �ൿ�� �õ��ϴ°��� ���� ���� flag�Դϴ�. flag : CAN_BEHAVIOR, CAN_NOT_BEHAVIOR

	/*
	 *  Gun state
	 */

	const short CAN_FIRE = 1;
	const short CAN_NOT_FIRE = 0;
	short mFireFlag;		// �ϳ��� �����常 �����ϵ��� �ϱ�����  flag �����Դϴ�. flag : CAN_FIRE, CAN_NOT_FIRE
	// �ִ��ν��Ͻ��� ������Ʈ�ӽſ��� �Ʒ� 5�� ������ ���� �����մϴ�.
	bool mbNeedBoltAction;
	bool mbIsEmptyMag;
	bool mbIsAiming;
	bool mbFiring;
	bool mbIsReload;
	EWaeponFireMode mFireMode;

	/*
	 *  Gun info
	 */
	FTimerHandle mFireDelayTimer;
	FTimerHandle mBurstFireTimer;
	const int FIRE_SWITCH_MODE = 3;
	int32 mFireModeOffset;
	int32 mMaxMagAmount;
	int32 mRemainAmmo;
	int32 mCurrentBurst;
	float mFireDelay;

	// gun recoil
	float mRecoilFactor;
	bool mFirstShot;
	int32 mContinuousShots;

};

