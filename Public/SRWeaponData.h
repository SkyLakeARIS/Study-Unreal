// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Version.h"
#include "GameModeData.h"
#include "GameFramework/Actor.h"
#include "SRWeaponData.generated.h"


/*
 * 무기 클래스의 정보를 가지는 데이터 클래스입니다.
 * 게임을 시작하기 전에 선택한 무기나 조준경등 게임모드를
 * 통해서 적절한 데이터를 반환합니다.
 */

UCLASS()
class VERSION_API ASRWeaponData final : public AActor
{
	GENERATED_BODY()

public:

	ASRWeaponData();

	/*
	 *  getter gun
	 */
	USkeletalMesh* GetWeapon(eWeaponType weapon) const;

	/*
	 *  getter scope
	 */
	TSubclassOf<AActor> GetScope(eScopeType scope) const;
	UChildActorComponent* GetScopeActor(eScopeType scope) const;

	/*
	 *  getter sound
	 */
	USoundBase* GetFireSound(eWeaponType weapon) const;
	USoundBase* GetDryFireSound() const;
	USoundBase* GetSwtichFireModeSound() const;

	/*
	 *  getter AnimInstance
	 */
	TSubclassOf<UAnimInstance> GetCharacterAnimInstance(eWeaponType weapon) const;
	TSubclassOf<UAnimInstance> GetWeaponAnimInstance(eWeaponType weapon) const;

	/*
	 *  getter effect
	 */
	UParticleSystem* GetMuzzleParticles(eWeaponType weapon) const;

protected:

	virtual void BeginPlay() override;

protected:

	UPROPERTY()
	USceneComponent* mDefault;

	/*
	 *  guns
	 */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Rifle)
	USkeletalMesh* mM4;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = HandGun)
	USkeletalMesh* mHandGun;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Sniper)
	USkeletalMesh* mM24;

	/*
	 *  optics
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Optics)
	UChildActorComponent* mScope1X;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Optics)
	UChildActorComponent* mScope2dot5X;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Optics)
	UChildActorComponent* mScope6X;

	UPROPERTY(VisibleAnywhere, Category = Optics)
	TSubclassOf<AActor> mHGOptic;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Optics)
	TSubclassOf<AActor> mScope1XClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Optics)
	TSubclassOf<AActor> mScope2dot5XClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Optics)
	TSubclassOf<AActor> mScope6XClass;


	/*
	 *  sounds
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sniper)
	USoundBase* mSniperFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rifle)
	USoundBase* mRifleFireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rifle)
	USoundBase* mSwitchFireModeSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HandGun)
	USoundBase* mHandGunFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Common)
	USoundBase* mDryFireSound;

	/*
	 * character animInstances
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance|Character")
	TSubclassOf<UAnimInstance> mCharacterSniperAnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance|Character")
	TSubclassOf<UAnimInstance> mCharacterRifleAnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance|Character")
	TSubclassOf<UAnimInstance> mCharacterHandGunAnimInstance;

	/*
	 * gun animInstances
	 */ 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance|Weapon")
	TSubclassOf<UAnimInstance> mSniperAnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance|Weapon")
	TSubclassOf<UAnimInstance> mRifleAnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance|Weapon")
	TSubclassOf<UAnimInstance> mHandGunAnimInstance;

	/*
	 *  effects
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles|Weapon")
	UParticleSystem* mSniperMuzzleParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles|Weapon")
	UParticleSystem* mRifleMuzzleParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles|Weapon")
	UParticleSystem* mHandGunMuzzleParticles;


};
