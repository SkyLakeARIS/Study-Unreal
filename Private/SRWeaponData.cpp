// Fill out your copyright notice in the Description page of Project Settings.
#include "SRWeaponData.h"

// Sets default values
ASRWeaponData::ASRWeaponData()
{
	// weapon skeletal mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Sniper(TEXT("/Game/Assets/Weapons/Sniper/Meshes/M24.M24"));
	if (SK_Sniper.Succeeded())
	{
		mM24 = SK_Sniper.Object;
	}
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Rifle(TEXT("/Game/Assets/Weapons/Rifle/Meshes/M4.M4"));
	if (SK_Rifle.Succeeded())
	{
		mM4 = SK_Rifle.Object;
	}
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_HandGun(TEXT("/Game/Assets/Weapons/Pistol/Meshes/pistol.pistol"));
	if (SK_HandGun.Succeeded())
	{
		mHandGun = SK_HandGun.Object;
	}

	mDefault = CreateDefaultSubobject<USceneComponent>(TEXT("scene"));
	mScope1X = CreateDefaultSubobject<UChildActorComponent>(TEXT("1XScope"));
	mScope2dot5X = CreateDefaultSubobject<UChildActorComponent>(TEXT("2dot5XScope"));
	mScope6X = CreateDefaultSubobject<UChildActorComponent>(TEXT("6XScope"));

	// scope childactor(blueprint actor)
	static ConstructorHelpers::FClassFinder<AActor> CA_Scope1X(TEXT("/Game/Assets/Weapons/BP_Optic.BP_Optic_C"));
	if (CA_Scope1X.Succeeded())
	{
		mScope1XClass = CA_Scope1X.Class;
	}
	static ConstructorHelpers::FClassFinder<AActor> CA_Scope2dot5X(TEXT("/Game/Assets/Weapons/BP_2XScope.BP_2XScope_C"));
	if (CA_Scope2dot5X.Succeeded())
	{
		mScope2dot5XClass = CA_Scope2dot5X.Class;
	}
	static ConstructorHelpers::FClassFinder<AActor> CA_Scope6X(TEXT("/Game/Assets/Weapons/BP_Scope.BP_Scope_C"));
	if (CA_Scope6X.Succeeded())
	{
		mScope6XClass = CA_Scope6X.Class;
	}


	// gun sounds
	static ConstructorHelpers::FObjectFinder<USoundBase> SB_SniperFire(TEXT("/Game/Assets/Sounds/m24fire.m24fire"));
	if (SB_SniperFire.Succeeded())
	{
		mSniperFireSound = SB_SniperFire.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> SB_RifleFire(TEXT("/Game/MilitaryWeapDark/Sound/Rifle/Wavs/RifleB_Fire02.RifleB_Fire02"));
	if (SB_RifleFire.Succeeded())
	{
		mRifleFireSound = SB_RifleFire.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> SB_RifleChangeFireMode(TEXT("/Game/Assets/Sounds/FireModeSwitch.FireModeSwitch"));
	if (SB_RifleChangeFireMode.Succeeded())
	{
		mSwitchFireModeSound = SB_RifleChangeFireMode.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> SB_HandGunFire(TEXT("/Game/Assets/Sounds/G26Fire.G26Fire"));
	if (SB_HandGunFire.Succeeded())
	{
		mHandGunFireSound = SB_HandGunFire.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> SB_DryFireSound(TEXT("/Game/Assets/Sounds/dryfire.dryfire"));
	if (SB_DryFireSound.Succeeded())
	{
		mDryFireSound = SB_DryFireSound.Object;
	}

	// character animInstance
	static ConstructorHelpers::FClassFinder<UAnimInstance> AI_CharacterSniper(TEXT("/Game/AnimBP_Sniper.AnimBP_Sniper_C"));
	if (AI_CharacterSniper.Succeeded())
	{
		mCharacterSniperAnimInstance = AI_CharacterSniper.Class;
	}
	static ConstructorHelpers::FClassFinder<UAnimInstance> AI_CharacterRifle(TEXT("/Game/AnimBP_C++.AnimBP_C++_C"));
	if (AI_CharacterRifle.Succeeded())
	{
		mCharacterRifleAnimInstance = AI_CharacterRifle.Class;
	}
	static ConstructorHelpers::FClassFinder<UAnimInstance> AI_CharacterHandGun(TEXT("/Game/AnimBP_HandGun.AnimBP_HandGun_C"));
	if (AI_CharacterHandGun.Succeeded())
	{
		mCharacterHandGunAnimInstance = AI_CharacterHandGun.Class;
	}

	// gun animInstance
	static ConstructorHelpers::FClassFinder<UAnimInstance> AI_Sniper(TEXT("/Game/GunAnimBP_Sniper.GunAnimBP_Sniper_C"));
	if (AI_Sniper.Succeeded())
	{
		mSniperAnimInstance = AI_Sniper.Class;
	}
	static ConstructorHelpers::FClassFinder<UAnimInstance> AI_Rifle(TEXT("/Game/GunAnimBP_Rifle.GunAnimBP_Rifle_C"));
	if (AI_Rifle.Succeeded())
	{
		mRifleAnimInstance = AI_Rifle.Class;
	}
	static ConstructorHelpers::FClassFinder<UAnimInstance> AI_HandGun(TEXT("/Game/GunAnimBP_HandGun.GunAnimBP_HandGun_C"));
	if (AI_HandGun.Succeeded())
	{
		mHandGunAnimInstance = AI_HandGun.Class;
	}


	// muzzle particles
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_RifleMuzzleParticles(TEXT("/Game/MilitaryWeapDark/FX/P_AssaultRifle_MuzzleFlash.P_AssaultRifle_MuzzleFlash"));
	if (PS_RifleMuzzleParticles.Succeeded())
	{
		mRifleMuzzleParticles = PS_RifleMuzzleParticles.Object;
	}

	// ar과 같은 이펙트를 공유합니다.
	if (PS_RifleMuzzleParticles.Succeeded())
	{
		mSniperMuzzleParticles = PS_RifleMuzzleParticles.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_HandGunMuzzleParticles(TEXT("/Game/MilitaryWeapDark/FX/P_Pistol_MuzzleFlash_01.P_Pistol_MuzzleFlash_01"));
	if (PS_HandGunMuzzleParticles.Succeeded())
	{
		mHandGunMuzzleParticles = PS_HandGunMuzzleParticles.Object;
	}
}

USkeletalMesh* ASRWeaponData::GetWeapon(EWeaponType weapon) const
{
	switch (weapon)
	{
	case EWeaponType::AR:
		return mM4;
	case EWeaponType::HG:
		return mHandGun;
	case EWeaponType::SR:
		return mM24;
	default:
		checkf(false, TEXT("ASRWeaponData-GetWeapon : 잘못된 EWeaponType 타입입니다."));
		break;
	}
	return nullptr;
}

TSubclassOf<AActor> ASRWeaponData::GetScope(EScopeType scope) const
{
	switch (scope)
	{
	case EScopeType::Scope1X:
		return mScope1XClass;
	case EScopeType::Scope2dot5X:
		return mScope2dot5XClass;
	case EScopeType::Scope6X:
		return mScope6XClass;
	default:
		checkf(false, TEXT("SRWeapon : GetScope - 올바르지 않은 enum타입입니다."));
		break;
	}

	return nullptr;
}

UChildActorComponent* ASRWeaponData::GetScopeActor(EScopeType scope) const
{

	switch (scope)
	{
	case EScopeType::Scope1X:
		return mScope1X;
	case EScopeType::Scope2dot5X:
		return mScope2dot5X;
	case EScopeType::Scope6X:
		return mScope6X;
	default:
		checkf(false, TEXT("SRWeapon : GetScope - 올바르지 않은 enum타입입니다."));
		break;
	}
	return nullptr;
}


USoundBase* ASRWeaponData::GetFireSound(EWeaponType weapon) const
{
	switch (weapon)
	{
	case EWeaponType::AR:
		return mRifleFireSound;
	case EWeaponType::HG:
		return mHandGunFireSound;
	case EWeaponType::SR:
		return mSniperFireSound;
	default:
		checkf(false, TEXT("ASRWeaponData-GetFireSound : 잘못된 EWeaponType 타입입니다."));
		break;
	}
	return nullptr;
}

USoundBase* ASRWeaponData::GetDryFireSound() const
{
	checkf(mDryFireSound != nullptr, TEXT("ASRWeaponData-GetDryFireSound : DryFireSound이 nullptr입니다."));
	return mDryFireSound;
}

USoundBase* ASRWeaponData::GetSwtichFireModeSound() const
{
	checkf(mSwitchFireModeSound != nullptr, TEXT("ASRWeaponData-GetSwtichFireModeSound : mSwitchFireModeSound이 nullptr입니다."));
	return mSwitchFireModeSound;
}

TSubclassOf<UAnimInstance> ASRWeaponData::GetCharacterAnimInstance(EWeaponType weapon) const
{
	switch (weapon)
	{
	case EWeaponType::AR:
		return mCharacterRifleAnimInstance;
	case EWeaponType::HG:
		return mCharacterHandGunAnimInstance;
	case EWeaponType::SR:
		return mCharacterSniperAnimInstance;
	default:
		checkf(false, TEXT("ASRWeaponData-GetCharacterAnimInstance : 잘못된 EWeaponType 타입입니다."));
		break;
	}
	return nullptr;
}

TSubclassOf<UAnimInstance> ASRWeaponData::GetWeaponAnimInstance(EWeaponType weapon) const
{
	switch (weapon)
	{
	case EWeaponType::AR:
		return mRifleAnimInstance;
	case EWeaponType::HG:
		return mHandGunAnimInstance;
	case EWeaponType::SR:
		return mSniperAnimInstance;
	default:
		checkf(false, TEXT("ASRWeaponData-GetWeaponAnimInstance : 잘못된 EWeaponType 타입입니다."));
		break;
	}
	return nullptr;
}

UParticleSystem* ASRWeaponData::GetMuzzleParticles(EWeaponType weapon) const
{
	switch (weapon)
	{
	case EWeaponType::AR:
		return mRifleMuzzleParticles;
	case EWeaponType::HG:
		return mHandGunMuzzleParticles;
	case EWeaponType::SR:
		return mSniperMuzzleParticles;
	default:
		checkf(false, TEXT("ASRWeaponData-GetWeaponAnimInstance : 잘못된 EWeaponType 타입입니다."));
		break;
	}
	return nullptr;
}

void ASRWeaponData::BeginPlay()
{
	Super::BeginPlay();

	mScope1X->SetChildActorClass(mScope1XClass);
	mScope2dot5X->SetChildActorClass(mScope2dot5XClass);
	mScope6X->SetChildActorClass(mScope6XClass);
}
