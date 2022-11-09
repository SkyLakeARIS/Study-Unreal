// Fill out your copyright notice in the Description page of Project Settings.
#include "SRWeapon.h"
#include "DrawDebugHelpers.h"
#include "SRAnimInstance.h"
#include "SRGameMode.h"
#include "SRHandGunBullet.h"
#include "SRPlayerCharacter.h"
#include "SRPlayerController.h"
#include "SRRifleBullet.h"
#include "SRSniperBullet.h"
#include "SRWeaponData.h"
#include "UIHUDWidget.h"
#include "Components/SceneCaptureComponent2D.h"

bool ASRWeapon::bIsDebugMode = false;

ASRWeapon::ASRWeapon()
{
	mWeaponData = CreateDefaultSubobject<ASRWeaponData>(TEXT("WeaponData"));
	mWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	mScope = CreateDefaultSubobject<UChildActorComponent>(TEXT("Scope6X"));

	mbFirstShot = true;
}

eWaeponFireMode ASRWeapon::GetFireMode() const
{
	return mFireMode;
}

int32 ASRWeapon::GetRemainAmmo() const
{
	return mRemainAmmo;
}

bool ASRWeapon::IsBurstShot() const
{
	if(mCurrentBurst > 0)
	{
		return true;
	}
	return false;
}

void ASRWeapon::BeginPlay()
{
	Super::BeginPlay();
}


void ASRWeapon::Initialize(const eGameType gameType, const eScopeType scopeType, const eWeaponType weaponType, ASRPlayerCharacter& owner)
{
	bIsDebugMode = false;
	
	mGameType = gameType;
	mScopeType = scopeType;
	mWeaponType = weaponType;

	mOwner = &owner;
	auto* const playerController = Cast<ASRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	mHUD = playerController->GetHUDWidget();

	mAddFireShot.BindUObject(playerController->GetPlayerState(), &ASRPlayerState::OnAddFireShots);

	mWeaponData = GetWorld()->SpawnActor<ASRWeaponData>(ASRWeaponData::StaticClass());


	 // 조준경에 따른 소켓 위치를 결정합니다.
	switch (mScopeType)
	{
	case eScopeType::Scope1X:
		mScopeLocationSocketName = FString("S_Scope1X");
		break;
	case eScopeType::Scope2dot5X:
		mScopeLocationSocketName = FString("S_Scope2dot5X");
		break;
	case eScopeType::Scope6X:
		mScopeLocationSocketName = FString("S_Scope6X");
		break;
	default:
		checkf(false, TEXT("Initialize - 올바르지 않은 enum eScopeType 데이터입니다."));
		break;
	}

	 // 총에 맞는 소켓 위치를 결정합니다.
	switch (mWeaponType)
	{
	case eWeaponType::AR:
		mWeaponLocationSocketName = FString("S_HandR_Rifle");
		break;
	case eWeaponType::HG:
		mWeaponLocationSocketName = FString("S_HandR_HandGun");
		break;
	case eWeaponType::SR:
		mWeaponLocationSocketName = FString("S_HandR_Sniper");
		break;
	default:
		checkf(false, TEXT("Initialize - 올바르지 않은 enum eWeaponType 데이터입니다."));
		break;
	}

	// 캐릭터와 총에 맞는 AnimInstance를 초기화 합니다.
	mWeapon->SetAnimInstanceClass(mWeaponData->GetWeaponAnimInstance(mWeaponType));
	mOwner->GetMesh1P()->SetAnimInstanceClass(mWeaponData->GetCharacterAnimInstance(mWeaponType));

	// 선택한 무기와 조준경을 장착합니다.
	mWeapon->SetSkeletalMesh(mWeaponData->GetWeapon(mWeaponType));
	mWeapon->AttachToComponent(mOwner->GetMesh1P(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), *mWeaponLocationSocketName);	// ik_hand

	mScope = (mWeaponData->GetScopeActor(mScopeType));
	mScope->AttachToComponent(mWeapon, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), *mScopeLocationSocketName);

	// 조준시 FOV 값이 조정되지 않는 타르코프 모드와 다른 모드간의 FOV값 체감을 덜기 위해 별개로 FOV값을 조정합니다.
	// 타모드는 조준시 카메라에도 FOV값 조정이 있어 상대적으로 타르코프 모드의 배율이 낮아보이는 느낌을 줄입니다.
	if (mGameType == eGameType::Tarkov)
	{
		auto* sceneCapture = Cast<USceneCaptureComponent2D>(mScope->GetChildComponent(0)->GetChildComponent(0));

		if (mScopeType == eScopeType::Scope6X)
		{
			sceneCapture->FOVAngle = 2.0f;
			mScope->GetChildActor()->SetActorRotation(mScope->GetChildActor()->GetActorRotation() + FRotator(-0.4f, 0.0f, 0.0f));
		}
		else if (mScopeType == eScopeType::Scope2dot5X)
		{
			sceneCapture->FOVAngle = 7.0f;
		}
	}

	// 두 조준경의 회전 정렬을 해줍니다.
	if (mScopeType == eScopeType::Scope1X)
	{
		mScope->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
	else if (mScopeType == eScopeType::Scope2dot5X)
	{
		mScope->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	}

	// 사운드를 초기화합니다.
	// 모든 총기에 공통으로 사용되는 사운드입니다.
	mFireSound = mWeaponData->GetFireSound(mWeaponType);
	mDryFireSound = mWeaponData->GetDryFireSound();

	// 파티클 이펙트를 초기화합니다.
	mMuzzleParticles = mWeaponData->GetMuzzleParticles(mWeaponType);

	// 총에 따른 별도의 초기화를 합니다.
	// 탄창의 크기, 초기 발사모드, 각각의 전용 사운드입니다.
	enum { SINGLE = 1, FULL_AUTO = 2, BURST = 3};
	switch (mWeaponType)
	{
	case eWeaponType::AR:
	{
		mMaxMagAmount = eWeaponMagSize::AR;
		mRemainAmmo = mMaxMagAmount;
		mFireDelay = 0.1f;
		mFireMode = eWaeponFireMode::FULL_AUTO;
		mProjectileClass = ASRRifleBullet::StaticClass();
		mFireModeOffset = FULL_AUTO;
		mCurrentBurst = BURST;
		mSwitchFireModeSound = mWeaponData->GetSwtichFireModeSound();
		break;
	}
	case eWeaponType::HG:
	{
		mMaxMagAmount = eWeaponMagSize::HG;
		mRemainAmmo = mMaxMagAmount;
		mFireDelay = 1.0f;
		mFireMode = eWaeponFireMode::SINGLE_FIRE;
		mProjectileClass = ASRHandGunBullet::StaticClass();
		mFireModeOffset = SINGLE;

		break;
	}
	case eWeaponType::SR:
	{
		mMaxMagAmount = eWeaponMagSize::SR;
		mRemainAmmo = mMaxMagAmount;
		mFireDelay = 2.0f;
		mFireMode = eWaeponFireMode::SINGLE_FIRE;
		mProjectileClass = ASRSniperBullet::StaticClass();
		mFireModeOffset = SINGLE;

		// 저격총의 경우 조준경의 크기가 비정상적으로 커지는 문제로 인해서 강제로 크기를 조정합니다.
		mScope->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
		break;
	}
	default:
		checkf(false, TEXT("InitGameMode : InitGameMode - 올바르지 않은 enum eWeaponType 데이터입니다."));
		break;
	}


	mHUD->UpdateAmmo(mRemainAmmo);
	mHUD->UpdateFireMode(mFireMode);
}


void ASRWeapon::StartFire()
{
	// 단발을 제외한 나머지 발사 모드는 타이머 작동. 점사만 따로 타이머, 카운트에 대해 처리.
	if(mFireMode != eWaeponFireMode::SINGLE_FIRE)
	{
		GetWorld()->GetTimerManager().SetTimer(mFireDelayTimer, this, &ASRWeapon::fireShots, mFireDelay, true, 0.0f);
	}
	else
	{
		fireShots();
	}
}

void ASRWeapon::StopFire()
{
	mbFirstShot = true;
	mRecoilFactor = 0.0f;
	mCurrentBurst = BURST_COUNT;
	GetWorld()->GetTimerManager().ClearTimer(mFireDelayTimer);
}

void ASRWeapon::Reload()
{
	StopFire();

}

void ASRWeapon::EndReload()
{
	mRemainAmmo = mMaxMagAmount;
	mHUD->UpdateAmmo(mRemainAmmo);

	if (mWeaponType == eWeaponType::SR && mOwner->GetBehaviorFlag() & FULL_RELOADING)
	{
		moveSocketSniperMode(false);
	}

	if(mOwner->GetBehaviorFlag() & FULL_RELOADING)
	{
		mOwner->UnsetBehaviorFlag(FULL_RELOADING);
	}
	else
	{
		mOwner->UnsetBehaviorFlag(RELOADING);
	}
	// 동작간에 딜레이를 주어서 애니메이션이 제대로 종료되고, 애니메이션과 변수들이 제대로 동작하도록합니다.
	FTimerHandle behaviorTimer;
	GetWorldTimerManager().SetTimer(behaviorTimer, this, &ASRWeapon::clearBehaviorFlagAfterAnimation, 0.5f, false);
}

void ASRWeapon::SwitchFireMode()
{
	if (mWeaponType != eWeaponType::AR)
	{
		return;
	}

	// 이전 발사 모드의 타이머가 남아있을 수 있으므로 제거합니다.
	GetWorld()->GetTimerManager().ClearTimer(mFireDelayTimer);
	eWaeponFireMode modes[] = { eWaeponFireMode::SINGLE_FIRE, eWaeponFireMode::BURST_FIRE,eWaeponFireMode::FULL_AUTO };
	mFireModeOffset = (mFireModeOffset + 1) % FIRE_SWITCH_MODE;
	mFireMode = modes[mFireModeOffset];

	UGameplayStatics::PlaySoundAtLocation(this, mSwitchFireModeSound, GetActorLocation());
	mHUD->UpdateFireMode(mFireMode);
}

void ASRWeapon::NotifyBeginBoltaction()
{
	moveSocketSniperMode(true);
}

void ASRWeapon::NotifyEndBoltaction()
{
	moveSocketSniperMode(false);
	mOwner->UnsetBehaviorFlag(BOLTACTION);

	// 동작간에 딜레이를 주어서 애니메이션이 제대로 종료되고, 애니메이션과 변수들이 제대로 동작하도록합니다.
	FTimerHandle behaviorTimer;
	GetWorldTimerManager().SetTimer(behaviorTimer, this, &ASRWeapon::clearBehaviorFlagAfterAnimation, 0.7f, false);
}

void ASRWeapon::clearBehaviorFlagAfterAnimation() const
{
	mOwner->UnsetBehaviorFlag(WAIT_TO_END_ANIMATION);
}

/*
 *  sniper 전용 동작 함수입니다.
 *	true - 볼트액션, 풀리로드를 수행할 때 총의 소켓을 일시적으로 손에서 다른 소켓으로 옮깁니다.
 *
 *	false - 총 소켓을 원상복귀합니다.
 *	추가적으로 mbNeedBoltAction변수를 false, mbCanFire변수를 true로 설정합니다.
 */
void ASRWeapon::moveSocketSniperMode(bool active)
{
	if (mWeaponType != eWeaponType::SR)
	{
		return;
	}

	if (active)
	{
		mWeapon->AttachToComponent(mOwner->GetMesh1P(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("S_Sniper"));
	}
	else
	{
		mWeapon->AttachToComponent(mOwner->GetMesh1P(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), *mWeaponLocationSocketName);
	}
}

void ASRWeapon::fireShots()
{
	// fire안에서 setRecoil 처리.
	USRAnimInstance* const animInstance = mOwner->GetAnimInstance();
	animInstance->Fire();

	// 반동 계수입니다. 스프레이의 반지름에 곱해집니다.
	// 0.1~1.0 사이로, 총알이 연속으로 발사될수록 계수가 높아집니다.
	// 권총은 계수가 그대로 유지됩니다.
	if (mbFirstShot)
	{
		mRecoilFactor = 0.0f;
		mbFirstShot = false;
	}
	else
	{
		mRecoilFactor += 0.1f;
		mRecoilFactor = FMath::Clamp(mRecoilFactor, 0.0f, 1.0f);

		if (mFireMode == eWaeponFireMode::SINGLE_FIRE)
		{
			mRecoilFactor = 0.15f;
			if (mWeaponType == eWeaponType::SR)
			{
				mRecoilFactor = 2.0f;
			}
		}
	}

	FVector muzzleLocation = FVector::ZeroVector;
	FVector endTrace = FVector::ZeroVector;
	FActorSpawnParameters rules;
	rules.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASRProjectile* projectile = nullptr;
	UCameraComponent* cameraComponent = mOwner->GetFirstPersonCameraComponent();
	switch (mGameType)
	{
	case eGameType::Battlefield:
	{
		// 총알이 캐릭터 카메라에서 발사되고, 투사체 방식입니다. (4, 2042시리즈와 비슷합니다.)
		// 랜덤 스프레이 방식입니다. 계산식은 5시리즈의 계산식이 사용되었습니다.
		// 총알의 스프레이 범위를 계산합니다.
		FRotator randSpread = FRotator::ZeroRotator;

		if (mOwner->GetBehaviorFlag() & AIMING)
		{
			randSpread.Yaw = FMath::Sqrt(FMath::RandRange(0.0f, 1.5f * mRecoilFactor)) * (FMath::Cos(FMath::FRandRange(0.0f, 3 * PI * mRecoilFactor)));
			randSpread.Pitch = FMath::Sqrt(FMath::RandRange(0.0f, 1.5f * mRecoilFactor)) * (FMath::Sin(FMath::FRandRange(0.0f, 3 * PI * mRecoilFactor)));
		}
		else
		{
			randSpread.Yaw = FMath::RandRange(-5.0f * mRecoilFactor, 5.0f * mRecoilFactor);
			randSpread.Pitch = FMath::RandRange(-5.0f * mRecoilFactor, 5.0f * mRecoilFactor);
		}

		muzzleLocation = cameraComponent->GetComponentLocation() + (cameraComponent->GetForwardVector() * 50.0f);
		projectile = GetWorld()->SpawnActor<ASRProjectile>(mProjectileClass, muzzleLocation, mOwner->GetControlRotation() + randSpread, rules);
		break;
	}
	case eGameType::RainbowSix:
	{
		// 총알이 캐릭터 카메라에서 발사되고, 히트스캔 방식입니다.
		// 랜덤 스프레이 방식입니다.
		// 총알의 스프레이 범위를 계산합니다.
		// 히트 스캔 방식이므로 끝지점 위치를 계산합니다.
		float spreadRadius = (mOwner->GetBehaviorFlag() & AIMING) ? 0.05f * mRecoilFactor : 0.05f;
		FVector2D randSpread = FMath::RandPointInCircle(spreadRadius);
		FVector spread(randSpread.X, 0.0f, randSpread.Y);

		muzzleLocation = mScope->GetChildComponent(0)->GetSocketLocation(FName("S_Aim"));
		endTrace = muzzleLocation + (cameraComponent->GetForwardVector() + spread) * 10000.0f;

		FHitResult hit;
		const FCollisionQueryParams queryParams = FCollisionQueryParams(SCENE_QUERY_STAT(WeaponTrace), false, this);
		if (GetWorld()->LineTraceSingleByChannel(hit, muzzleLocation, endTrace, ECC_EngineTraceChannel3, queryParams))
		{
			projectile = GetWorld()->SpawnActor<ASRProjectile>(mProjectileClass, hit.Location, mOwner->GetControlRotation(), rules);
			projectile->SetLifeSpan(2.0f);
			if (bIsDebugMode)
			{
				DrawDebugLine(GetWorld(), muzzleLocation, hit.ImpactPoint, FColor::Red, false, 15.0f);
			}
		}
		break;
	}
	case eGameType::Tarkov:
	{
		// 총알이 총구에서 나가는 방식입니다.
		muzzleLocation = mWeapon->GetSocketTransform(FName("S_Muzzle")).GetLocation();
		FRotator muzzleRotation = mWeapon->GetSocketTransform(FName("S_Muzzle")).Rotator();

		projectile = GetWorld()->SpawnActor<ASRProjectile>(mProjectileClass, muzzleLocation, muzzleRotation, rules);
		break;
	}
	default:
		checkf(false, TEXT("ASRPlayerCharacter- FireShot : 올바르지 않은 enum gametype 데이터입니다."));
		break;
	}

	// R6S 방식에서 ray가 아무것도 충돌을 감지하지 못하면 projectile이 생성되지 않으므로
	// nullptr가 발생함으로 허공에 발사시에 대한 안전장치.
	auto* const playerController = Cast<ASRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (projectile != nullptr)
	{
		projectile->SetStartLocation(muzzleLocation);
		projectile->SetBulletType(mWeaponType);
		projectile->BindPlayerStateInfo(*playerController->GetPlayerState());
		projectile->BindHUDWidget(*mHUD);
	}


	--mRemainAmmo;
	mAddFireShot.Execute();
	mHUD->UpdateAmmo(mRemainAmmo);


	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), mMuzzleParticles, mWeapon->GetSocketTransform(FName("S_Muzzle")).GetLocation(), mWeapon->GetSocketTransform(FName("S_Muzzle")).GetRotation().Rotator());

	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), mFireSound, GetActorLocation());


	if (mFireMode == eWaeponFireMode::SINGLE_FIRE)
	{
		animInstance->UnsetFire();
	}
	else if (mFireMode == eWaeponFireMode::BURST_FIRE)
	{
		--mCurrentBurst;
		if (mCurrentBurst <= 0)
		{
			animInstance->UnsetFire();
			mOwner->UnsetBehaviorFlag(FIRING);
			StopFire();
			// 트릭으로 딜레이 구현. (다른 행동을 하지 못하게 막았다 풀어주는 형식)
			mOwner->SetBehaviorFlag(WAIT_TO_END_ANIMATION);
			GetWorldTimerManager().SetTimer(mBurstDelayTimer, [&](){mOwner->UnsetBehaviorFlag(WAIT_TO_END_ANIMATION); }, BURST_DELAY, false);
		}
	}

	if (mRemainAmmo <= 0)
	{
		animInstance->UnsetFire();
		mCurrentBurst = 0;

		mOwner->UnsetBehaviorFlag(FIRING);
		mOwner->SetBehaviorFlag(FULL_RELOADING);
		mOwner->SetBehaviorFlag(WAIT_TO_END_ANIMATION);

		GetWorldTimerManager().ClearTimer(mFireDelayTimer);

	}
	else if (mWeaponType == eWeaponType::SR)
	{
		mOwner->SetBehaviorFlag(WAIT_TO_END_ANIMATION);

		animInstance->UnsetFire();
		mOwner->UnsetBehaviorFlag(FIRING);
		mOwner->SetBehaviorFlag(BOLTACTION);

	}
}
