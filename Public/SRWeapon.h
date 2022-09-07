// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "GameModeData.h"
#include "GameFramework/Actor.h"
#include "SRWeapon.generated.h"


/*
 * 무기 클래스입니다.
 * 게임을 시작하기 전에 선택된 무기 정보를 통해서
 * 해당 무기나 조준경등 게임모드를 통해서 적절한 데이터를 반환합니다.
 */

UCLASS()
class VERSION_API ASRWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ASRWeapon();

	USkeletalMesh* GetWeapon(EWeaponType weapon) const;

	TSubclassOf<AActor> GetScope(EScopeType scope) const;

	UChildActorComponent* GetScopeActor(EScopeType scope) const;

	USoundBase* GetFireSound(EWeaponType weapon) const;

	USoundBase* GetDryFireSound() const;

	USoundBase* GetSwtichFireModeSound() const;

	TSubclassOf<UAnimInstance> GetCharacterAnimInstance(EWeaponType weapon) const;

	TSubclassOf<UAnimInstance> GetWeaponAnimInstance(EWeaponType weapon) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY()
	USceneComponent* mDefault;

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
	USoundBase* DryFireSound;

	// character animInstance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance|Character")
	TSubclassOf<UAnimInstance> mCharacterSniperAnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance|Character")
	TSubclassOf<UAnimInstance> mCharacterRifleAnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance|Character")
	TSubclassOf<UAnimInstance> mCharacterHandGunAnimInstance;

	// gun animInstance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance|Weapon")
	TSubclassOf<UAnimInstance> mSniperAnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance|Weapon")
	TSubclassOf<UAnimInstance> mRifleAnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimInstance|Weapon")
	TSubclassOf<UAnimInstance> mHandGunAnimInstance;
};
