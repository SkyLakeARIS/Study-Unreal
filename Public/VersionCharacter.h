// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <atomic>
#include "Version.h"
#include "GameModeData.h"
#include "GameFramework/Character.h"
#include "VersionCharacter.generated.h"



enum class EHitType : uint8;
class UADAnimInstance;
UCLASS(config=Game)
class AVersionCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AVersionCharacter();

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
	bool IsFire() const;
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

	void AddViewPortHitMark(EHitType hitType);

	UFUNCTION()
	void InitGameMode(FGameModeData modeData);

	void SaveInGameSetting();
	void LoadInGameSetting();

	UFUNCTION(BlueprintCallable)
	void SetCanNotFire() {
		mbCanFire = false;
		atomic_thread_fence(std::memory_order_seq_cst);
	}
protected:

	void boltactionDelay() {
		mbCanFire = true;
		atomic_thread_fence(std::memory_order_seq_cst);
		mbNeedBoltAction = false;
		atomic_thread_fence(std::memory_order_seq_cst);
	}

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	void tryBindSelectModesUI();

	void clearHitMark();

	void StartFire();
	void StopFire();
	void BurstFire();
	void FireShot();

	void Reload();

	void SwitchFireMode();

	void SetAim();
	void SetHip();

	void MoveForward(float Val);
	void MoveRight(float Val);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

public:

	FMouseSensitivity MouseSetting;

protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Character)
	USkeletalMeshComponent* mMesh1P;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	UCameraComponent* mFirstPersonCameraComponent;

	UPROPERTY(BlueprintReadOnly)
	UADAnimInstance* mTutAnimInstance;
	UPROPERTY()
	class ACharacterPlayerController* mPlayerController;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AVersionProjectile> mProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UUserWidget> mCrossHairClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UUserWidget> mHitMarkClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UUserWidget> mHeadshotMarkClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UUserWidget> mKillMarkClass;

	UPROPERTY()
	UUserWidget* mCrossHair;
	UPROPERTY()
	UUserWidget* mHitMark;
	UPROPERTY()
	UUserWidget* mHeadshotMark;
	UPROPERTY()
	UUserWidget* mKillMark;


	FGameModeData mGameModeData;

	/*
	 *  Weapons and sights
	 */
	UPROPERTY()
	USkeletalMeshComponent* mWeapon;
	UPROPERTY()
	UChildActorComponent* mScope;

	FString mScopeLocationSocketName;
	FString mWeaponLocationSocketName;

	// sounds
	UPROPERTY()
	USoundBase* mFireSound;
	UPROPERTY()
	USoundBase* mSwitchFireModeSound;
	UPROPERTY()
	USoundBase* mDryFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Effect")
	class UParticleSystem* mMuzzleParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	TSubclassOf<class ASRWeapon> mWeaponDataClass;

	UPROPERTY()
	ASRWeapon* mWeaponData;

private:

	EAimingType mAimingType;

	// UI
	EHitType mCurrentMark;
	FTimerHandle mHitMarkTimer;

	/*
	 *  Gun state
	 */
	volatile bool mbNeedBoltAction;
	volatile bool mbIsEmptyMag;		// full reload || reload
	bool mbIsAiming;
	volatile bool mbFiring;			// true - fire, false - non fire
	std::atomic<bool> mbCanFire;			// false - reload  or need boltaction
	volatile bool mbIsReload;
	float mSumPitch;
	/*
	 *  Gun info
	 */
	int32 mMaxMagAmount;
	int32 mRemainAmmo;
	int32 CurrentBurst;
	float mARFireRate;

	// gun recoil
	float mRecoilFactor;
	bool mFirstShot;
	int32 mContinuousShots;

	FTimerHandle TimerHandle_BurstMode;
	FTimerHandle TimerHandle_FullAutoMode;
	FTimerHandle mBoltActionTimer;
	FTimerHandle mReloadDelayTimer; // 애니메이션 추가전까지 임시로 사용하는 변수

	EWaeponFireMode mCurrentFireMode;
	int32 FIRE_SWITCH_MODE;
	int32 FireModeOffset;

};

