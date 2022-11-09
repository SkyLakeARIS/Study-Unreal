// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "GameModeData.h"
#include "GameFramework/Actor.h"
#include "SRWeapon.generated.h"


DECLARE_DELEGATE(FOnAddFireShot);

class USRWeaponData;
class ASRPlayerCharacter;
class UUIHUDWidget;


/*
 * 무기 클래스 클래스입니다.
 * 게임을 시작하기 전에 선택한 무기나 조준경등 게임모드를
 * 통해서 적절한 데이터를 반환합니다.
 */
UCLASS()
class VERSION_API ASRWeapon final : public AActor
{
	GENERATED_BODY()
	
public:
	ASRWeapon();

	void StartFire();
	void StopFire();

	/*
	 * call by anim blueprints
	 */
	void Reload();
	UFUNCTION(BlueprintCallable)
	void EndReload();

	void SwitchFireMode();

	/*
	 *  call by anim blueprints
	 */
	UFUNCTION(BlueprintCallable)
	void NotifyBeginBoltaction();
	UFUNCTION(BlueprintCallable)
	void NotifyEndBoltaction();

	/*
	 *  setter
	 */

	void Initialize(const eGameType gameType, const eScopeType scopeType, const eWeaponType weaponType, ASRPlayerCharacter& owner);

	/*
	 *  getter
	 */
	UFUNCTION(BlueprintPure)
	USkeletalMeshComponent* GetGun()  const { return mWeapon; }
	UFUNCTION(BlueprintPure)
	UChildActorComponent* GetScope() const { return mScope; }
	eWaeponFireMode GetFireMode() const;
	int32 GetRemainAmmo() const;
	bool IsBurstShot() const;

protected:

	virtual void BeginPlay() override;

private:

	void moveSocketSniperMode(bool active);

	void fireShots();

	void clearBehaviorFlagAfterAnimation() const;

public :

	static bool bIsDebugMode;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	TSubclassOf<class ASRWeaponData> mWeaponDataClass;

private:

	/*
	 *  weapon data class
	 */

	UPROPERTY()
	ASRWeaponData* mWeaponData;

	/*
	 * weapon
	 */
	UPROPERTY()
	USkeletalMeshComponent* mWeapon;
	UPROPERTY()
	UChildActorComponent* mScope;

	FString mScopeLocationSocketName;
	FString mWeaponLocationSocketName;

	/*
	 *  projectile
	 */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ASRProjectile> mProjectileClass;

	/*
	 *  sounds and effect
	 */
	UPROPERTY()
	USoundBase* mFireSound;
	UPROPERTY()
	USoundBase* mSwitchFireModeSound;
	UPROPERTY()
	USoundBase* mDryFireSound;

	UPROPERTY()
	UParticleSystem* mMuzzleParticles;

	/*
	 *  class and ui
	 */
	UPROPERTY()
	UUIHUDWidget* mHUD;
	UPROPERTY()
	ASRPlayerCharacter* mOwner;

	/*
	 *  player state delegate
	 */
	FOnAddFireShot mAddFireShot;

	/*
	 *  gun info
	 */
	FTimerHandle mFireDelayTimer;
	float mFireDelay;
	float mRecoilFactor;
	bool mbFirstShot;
	/*
	 *  mag info
	 */
	int32 mMaxMagAmount;
	int32 mRemainAmmo;

	/*
	 *  fire mode info
	 */
	FTimerHandle mBurstDelayTimer;
	float BURST_DELAY = 0.4f;
	const int32 BURST_COUNT = 3;
	int32 mCurrentBurst;

	const int FIRE_SWITCH_MODE = 3;
	eWaeponFireMode mFireMode;
	int32 mFireModeOffset;

	/*
	 *  game mode info
	 */
	eGameType mGameType;
	eScopeType mScopeType;
	eWeaponType mWeaponType;
};



