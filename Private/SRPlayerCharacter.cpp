// Copyright Epic Games, Inc. All Rights Reserved.

#include "SRPlayerCharacter.h"
#include "SRPlayerController.h"
#include "SRAnimInstance.h"
#include "SRProjectile.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "SRHandGunBullet.h"
#include "SRInGameSetting.h"
#include "SRRifleBullet.h"
#include "SRSniperBullet.h"
#include "SRWeapon.h"
#include "UIHUDWidget.h"
#include "UISelectModesWidget.h"
#include "Components/SceneCaptureComponent2D.h"


DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ASRPlayerCharacter

ASRPlayerCharacter::ASRPlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(10.f, 96.0f);

	// Create a CameraComponent	
	mFirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	mFirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	mFirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	mFirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a 1st person view (when controlling this pawn)
	mMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	mMesh1P->SetOnlyOwnerSee(true);
	mMesh1P->SetupAttachment(mFirstPersonCameraComponent);
	mMesh1P->bCastDynamicShadow = false;
	mMesh1P->CastShadow = false;
	mMesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	mMesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	mWeaponData = CreateDefaultSubobject<ASRWeapon>(TEXT("WeaponData"));
	mWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	mScope = CreateDefaultSubobject<UChildActorComponent>(TEXT("Scope6X"));


	// UI 초기화

	static ConstructorHelpers::FClassFinder<UUserWidget> UI_Crosshair(TEXT("/Game/UI/UI_Crosshair.UI_Crosshair_C"));
	if (UI_Crosshair.Succeeded())
	{
		mCrossHairClass = UI_Crosshair.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HitMark(TEXT("/Game/UI/UI_HitMark.UI_HitMark_C"));
	if (UI_HitMark.Succeeded())
	{
		mHitMarkClass = UI_HitMark.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HeadshotMark(TEXT("/Game/UI/UI_HeadshotMark.UI_HeadshotMark_C"));
	if (UI_HeadshotMark.Succeeded())
	{
		mHeadshotMarkClass = UI_HeadshotMark.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_KillMark(TEXT("/Game/UI/UI_KillMark.UI_KillMark_C"));
	if (UI_KillMark.Succeeded())
	{
		mKillMarkClass = UI_KillMark.Class;
	}

	// 실제 게임 시작시 InitGameMode에서 초기화 됩니다.
	// character state
	mBehaviorFlag = CAN_BEHAVIOR;

	// 무기관련
	mFireModeOffset = 2;
	mRemainAmmo = 30;
	mFireDelay = 0.1f;
	mCurrentBurst = 0;
	mFireMode  = EWaeponFireMode::FULL_AUTO;

	// 무기 상태 관련
	mbIsAiming = false;
	mbIsEmptyMag = false;
	mbFiring = false;
	mbNeedBoltAction = false;
	mbIsReload = false;
	mFireFlag = CAN_FIRE;

	// 무기 반동
	mRecoilFactor = 0.0f;
	// 조준 방식
	mAimingType = EAimingType::Hold;

	// 소켓 위치
	mScopeLocationSocketName = FString("S_Scope1X");
	mWeaponLocationSocketName = FString("S_HandR_Rifle");
}

void ASRPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ASRPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASRPlayerCharacter::SetAimToggleOrHold(EAimingType newType)
{
	mAimingType = newType;
}

/*
 *  sniper 전용 동작 함수입니다.
 *	true - 볼트액션, 풀리로드를 수행할 때 총의 소켓을 일시적으로 손에서 다른 ik소켓으로 옮깁니다.
 *	
 *	false - 총 소켓을 원상복귀합니다.
 *	추가적으로 mbNeedBoltAction변수를 false, mbCanFire변수를 true로 설정합니다. 
 */
void ASRPlayerCharacter::SniperMoveSocket(bool active)
{
	if(mGameModeData.weapon != EWeaponType::SR)
	{
		return;
	}

	if(active)
	{
		mWeapon->AttachToComponent(mMesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("ik_hand"));
	}
	else
	{
		mWeapon->AttachToComponent(mMesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), *mWeaponLocationSocketName);
	}
}

// 볼트 액션이 동작이 끝나고 다시 사격이 가능해지면 호출됩니다. SniperAnimInstacne에서 호출됩니다.
void ASRPlayerCharacter::NotifyBoltActionEnd()
{
	mbNeedBoltAction = false;
	SniperMoveSocket(false);

	// 동작간에 딜레이를 주어서 애니메이션이 제대로 종료되고, 변수들이 제대로 동작하도록합니다.
	FTimerHandle behaviorTimer;
	FTimerHandle weaponTimer;
	GetWorldTimerManager().SetTimer(behaviorTimer, this, &ASRPlayerCharacter::endBehaviorDelay, 0.5f, false);
	GetWorldTimerManager().SetTimer(weaponTimer, this, &ASRPlayerCharacter::endWeaponDelay, 0.3f, false);
}

// 재장전 액션이 동작이 끝나고 다시 사격이 가능해지면 호출됩니다. 캐릭터AnimInstacne들에서 호출됩니다.
void ASRPlayerCharacter::NotifyReloadEnd()
{
	mRemainAmmo = mMaxMagAmount;
	mPlayerController->GetIngameHUD()->UpdateAmmo(mRemainAmmo);

	mbNeedBoltAction = false;

	mbIsEmptyMag = false;
	mbIsReload = false;

	if(mGameModeData.weapon == EWeaponType::SR)
	{
		SniperMoveSocket(false);
	}
	// 동작간에 딜레이를 주어서 애니메이션이 제대로 종료되고, 변수들이 제대로 동작하도록합니다.
	FTimerHandle behaviorTimer;
	FTimerHandle weaponTimer;
	GetWorldTimerManager().SetTimer(behaviorTimer, this, &ASRPlayerCharacter::endBehaviorDelay, 0.5f, false);
	GetWorldTimerManager().SetTimer(weaponTimer, this, &ASRPlayerCharacter::endWeaponDelay, 0.3f, false);
}


void ASRPlayerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	mPlayerController = Cast<ASRPlayerController>(GetController());

	EnableInput(mPlayerController);

	mWeaponData = GetWorld()->SpawnActor<ASRWeapon>(mWeaponDataClass);

	if (mCrossHairClass)
	{
		mCrossHair = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), mCrossHairClass);
	}

	mHitMark = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), mHitMarkClass);
	mHeadshotMark = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), mHeadshotMarkClass);
	mKillMark = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), mKillMarkClass);

	mCrossHair->AddToViewport();
	mCrossHair->SetVisibility(ESlateVisibility::Hidden);

	// 다른 클래스들의 초기화를 위해서 약간의 딜레이 후 데이터를 초기화 합니다.
	FTimerHandle bindTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(bindTimerHandle, this, &ASRPlayerCharacter::tryBindSelectModesUI, 1.0f, false, 1.0f);
	FTimerHandle loadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(loadTimerHandle, this, &ASRPlayerCharacter::LoadInGameSetting, 1.0f, false, 1.0f);
}

void ASRPlayerCharacter::tryBindSelectModesUI()
{
	mPlayerController->GetSelectModesWidget()->BindCharacterInfo(this);
}

// 타이머에 의해 호출되는 히트마크를 지우는 함수입니다.
void ASRPlayerCharacter::clearHitMark()
{
	switch (mCurrentMark)
	{
	case EHitType::Hit:
		mHitMark->RemoveFromParent();
		break;
	case EHitType::HeadShot:
		mHeadshotMark->RemoveFromParent();
		break;
	case EHitType::Kill:
		mKillMark->RemoveFromParent();
		break;
	}
	GetWorld()->GetTimerManager().ClearTimer(mHitMarkTimer);
}

void ASRPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASRPlayerCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASRPlayerCharacter::StopFire);
	PlayerInputComponent->BindAction("SwitchFireMode", IE_Pressed, this, &ASRPlayerCharacter::SwitchFireMode);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASRPlayerCharacter::Reload);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ASRPlayerCharacter::SetAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ASRPlayerCharacter::SetHip);

	//PlayerInputComponent->BindAxis("MoveForward", this, &ASRPlayerCharacter::MoveForward);
	//PlayerInputComponent->BindAxis("MoveRight", this, &ASRPlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this,&ASRPlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ASRPlayerCharacter::LookUpAtRate);
}

EWeaponType ASRPlayerCharacter::GetWeaponType() const
{
	return mGameModeData.weapon;
}

EScopeType ASRPlayerCharacter::GetScopeType() const
{
	return mGameModeData.scope;
}

EGameType ASRPlayerCharacter::GetGameType() const
{
	return mGameModeData.game;
}

// 타겟에 적중시 적절한 히트마크를 크로스헤어가 위치하는 곳에 표시하는 함수입니다.
// 표시할 히트마크의 유형은 projectile 개체가 전달해줍니다.
void ASRPlayerCharacter::AddViewPortHitMark(EHitType hitType)
{
	mCurrentMark = hitType;
	// 이미 히트마크가 존재하는것은 지우고 새로 갱신합니다.
	mHitMark->RemoveFromParent();
	mHeadshotMark->RemoveFromParent();
	mKillMark->RemoveFromParent();
	switch(mCurrentMark)
	{
		case EHitType::Hit:
			mHitMark->AddToViewport(1);
			break;
		case EHitType::HeadShot:
			mHeadshotMark->AddToViewport(1);
			break;
		case EHitType::Kill:
			mKillMark->AddToViewport(1);
			break;
		default:
			UE_LOG(LogTemp, Display, TEXT("ASRPlayerCharacter - AddViewPortHitMark : 올바르지 않은 EHitType 타입입니다."));
			break;
	}
	GetWorld()->GetTimerManager().SetTimer(mHitMarkTimer, this, &ASRPlayerCharacter::clearHitMark, 0.5f, false, -1.0f);
}

bool ASRPlayerCharacter::IsAimimg() const
{
	return mbIsAiming;
}

EAimingType ASRPlayerCharacter::GetAimingType() const
{
	return mAimingType;
}

bool ASRPlayerCharacter::IsCanFire() const
{
	if(mFireFlag == CAN_FIRE)
	{
		return true;
	}
	return false;
}

bool ASRPlayerCharacter::IsNeedBoltAction() const
{
	return mbNeedBoltAction;
}

bool ASRPlayerCharacter::IsFiring() const
{
	return mbFiring;
}

bool ASRPlayerCharacter::IsEmptyMag() const
{
	return mbIsEmptyMag;
}

bool ASRPlayerCharacter::IsReload() const
{
	return mbIsReload;
}

/*
 *  맵 선택 UI에서 인게임으로 들어올 때 무기와(총, 조준경) 게임모드를 선택하는 UI로부터
 *	데이터를 넘겨받아 캐릭터 데이터를 업데이트하는 함수입니다.
 */
void ASRPlayerCharacter::InitGameMode(FGameModeData modeData)
{
	mGameModeData = modeData;

	// 게임 모드 데이터를 초기화 합니다.
	FString DisplayMode;
	switch (mGameModeData.game)
	{
	case EGameType::Battlefield:
	{
		UE_LOG(LogTemp, Display, TEXT("InitGameMode : battlefield mode is applied"));
		DisplayMode = "battlefield mode";
		break;
	}
	case EGameType::RainbowSix:
	{
		UE_LOG(LogTemp, Display, TEXT("InitGameMode : rainbow six:siege mode is applied"));
		DisplayMode = "rainbow six:siege mode";
		break;
	}
	case EGameType::Tarkov:
	{
		UE_LOG(LogTemp, Display, TEXT("InitGameMode : Tarkov mode is applied"));
		DisplayMode = "Tarkov mode";
		break;
	}
	default:
		UE_LOG(LogTemp, Warning, TEXT("ASRPlayerCharacter - InitGameMode : 올바르지 않은 enum EGameType 데이터입니다."));
		break;
	}

	// 조준경에 따른 소켓 위치를 결정합니다..
	switch(mGameModeData.scope)
	{
		case EScopeType::Scope1X:
			mScopeLocationSocketName = FString("S_Scope1X");
			break;
		case EScopeType::Scope2dot5X:
			mScopeLocationSocketName = FString("S_Scope2dot5X");
			break;
		case EScopeType::Scope6X:
			mScopeLocationSocketName = FString("S_Scope6X");
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("InitGameMode : InitGameMode - 올바르지 않은 enum EScopeType 데이터입니다."));
			break;
	}

	if(mGameModeData.game != EGameType::Tarkov && mGameModeData.weapon != EWeaponType::SR)
	{
		mCrossHair->SetVisibility(ESlateVisibility::Visible);
	}

	// 총에 맞는 소켓 위치를 결정합니다.
	switch (mGameModeData.weapon)
	{
		case EWeaponType::AR:
			mWeaponLocationSocketName = FString("S_HandR_Rifle");
			break;
		case EWeaponType::HG:
			mWeaponLocationSocketName = FString("S_HandR_HandGun");
			break;
		case EWeaponType::SR:
			mWeaponLocationSocketName = FString("S_HandR_Sniper");
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("InitGameMode : InitGameMode - 올바르지 않은 enum EWeaponType 데이터입니다."));
			break;
	}

	// 캐릭터와 총에 맞는 AnimInstance를 초기화 합니다.
	mWeapon->SetAnimInstanceClass(mWeaponData->GetWeaponAnimInstance(mGameModeData.weapon));
	mMesh1P->SetAnimInstanceClass(mWeaponData->GetCharacterAnimInstance(mGameModeData.weapon));

	// 선택한 무기와 조준경을 장착합니다.
	mWeapon->SetSkeletalMesh(mWeaponData->GetWeapon(mGameModeData.weapon));
	mWeapon->AttachToComponent(mMesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), *mWeaponLocationSocketName);	// ik_hand

	mScope = (mWeaponData->GetScopeActor(mGameModeData.scope));
	mScope->AttachToComponent(mWeapon, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), *mScopeLocationSocketName);

	// 조준시 FOV 값이 조정되지 않는 타르코프 모드와 다른 모드간의 FOV값 체감을 덜기 위해 별개로 FOV값을 조정합니다.
	// 타모드는 조준시 카메라에도 FOV값 조정이 있어 상대적으로 타르코프 모드의 배율이 낮아보이는 느낌을 줄입니다.
	if (mGameModeData.game == EGameType::Tarkov)
	{
		auto sceneCapture = Cast<USceneCaptureComponent2D>(mScope->GetChildComponent(0)->GetChildComponent(0));

		if (mGameModeData.scope == EScopeType::Scope6X)
		{
			sceneCapture->FOVAngle = 2.0f;
			mScope->GetChildActor()->SetActorRotation(mScope->GetChildActor()->GetActorRotation() + FRotator(-0.4f, 0.0f, 0.0f));
		}
		else if (mGameModeData.scope == EScopeType::Scope2dot5X)
		{
			sceneCapture->FOVAngle = 7.0f;
		}
	}

	// 두 조준경의 회전 정렬을 해줍니다.
	if (mGameModeData.scope == EScopeType::Scope1X)
	{
		mScope->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
	else if (mGameModeData.scope == EScopeType::Scope2dot5X)
	{
		mScope->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	}

	// 사운드를 초기화합니다.
	// 모든 총기에 공통으로 사용되는 사운드입니다.
	mFireSound = mWeaponData->GetFireSound(mGameModeData.weapon);
	mDryFireSound = mWeaponData->GetDryFireSound();

	// 총에 따른 별도의 초기화를 합니다.
	// 탄창의 크기, 초기 발사모드, 각각의 전용 사운드입니다.
	switch(modeData.weapon)
	{
		case EWeaponType::AR:
			{
				mMaxMagAmount = EWeaponMagSize::AR;
				mRemainAmmo = mMaxMagAmount;
				mFireDelay = 0.1f;
				mFireMode = EWaeponFireMode::FULL_AUTO;
				mProjectileClass = ASRRifleBullet::StaticClass();

				mSwitchFireModeSound = mWeaponData->GetSwtichFireModeSound();
				break;
			}
		case EWeaponType::HG:
			{
				mMaxMagAmount = EWeaponMagSize::HG;
				mRemainAmmo = mMaxMagAmount;
				mFireDelay = 0.3f;
				mFireMode = EWaeponFireMode::SINGLE_FIRE;
				mProjectileClass = ASRHandGunBullet::StaticClass();

				break;
			}
		case EWeaponType::SR:
			{
				mMaxMagAmount = EWeaponMagSize::SR;
				mRemainAmmo = mMaxMagAmount;
				mFireDelay = 2.0f;
				mFireMode = EWaeponFireMode::SINGLE_FIRE;
				mProjectileClass = ASRSniperBullet::StaticClass();

				// 저격총의 경우 조준경의 크기가 비정상적으로 커지는 문제로 인해서 강제로 크기를 조정합니다.
				mScope->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
				break;
			}
		default:
			UE_LOG(LogTemp, Warning, TEXT("InitGameMode : InitGameMode - 올바르지 않은 enum EWeaponType 데이터입니다."));
			break;
	}

	// 초기화된 데이터들을 업데이트 합니다.
	mTutAnimInstance = Cast<USRAnimInstance>(mMesh1P->GetAnimInstance());
	mTutAnimInstance->UpdateSocketInfo();

	mPlayerController->GetIngameHUD()->UpdateGameMode(DisplayMode);
	mPlayerController->GetIngameHUD()->UpdateFireMode(mFireMode);
	mPlayerController->GetIngameHUD()->UpdateAmmo(mRemainAmmo);

	mFireFlag = CAN_FIRE;
	mBehaviorFlag = CAN_BEHAVIOR;
}

// 재장전 함수. AnimNotify에 의해서 다시 사격가능한 상태가 됩니다.
void ASRPlayerCharacter::Reload()
{
	if(_InterlockedCompareExchange16(&mBehaviorFlag, CAN_NOT_BEHAVIOR, CAN_BEHAVIOR) == CAN_BEHAVIOR)
	{
		if(mbIsAiming)
		{
			mbIsAiming = false;
			mTutAnimInstance->SetAiming(mbIsAiming);
		}
		mbIsReload = true;
	}
}

/*
 *	무기의 발사 모드를 변경하는 함수.
 *  권총과 저격총의 경우 Single모드 하나만 존재하므로
 *	소총만 발사모드가 작동됨.
 */
void ASRPlayerCharacter::SwitchFireMode()
{
	if(mGameModeData.weapon == EWeaponType::AR)
	{
		// 이전 발사 모드의 타이머가 남아있을 수 있으므로 제거합니다.
		GetWorld()->GetTimerManager().ClearTimer(mFireDelayTimer);
		EWaeponFireMode modes[] = { EWaeponFireMode::SINGLE_FIRE, EWaeponFireMode::BURST_FIRE,EWaeponFireMode::FULL_AUTO };
		mFireModeOffset = (mFireModeOffset + 1) % FIRE_SWITCH_MODE;
		mFireMode  = modes[mFireModeOffset];

		UGameplayStatics::PlaySoundAtLocation(this, mSwitchFireModeSound, GetActorLocation());
		mPlayerController->GetIngameHUD()->UpdateFireMode(mFireMode);
	}
}

void ASRPlayerCharacter::StartFire()
{

	// just read
	if (mBehaviorFlag == CAN_NOT_BEHAVIOR)
	{
		return;
	}

	if (mRemainAmmo <= 0)
	{
		UGameplayStatics::PlaySoundAtLocation(this, mDryFireSound, GetActorLocation());
		return;
	}

	// 빠르게 호출되는것을 막고, 무기별 사격간 딜레이를(부가 효과) 제공해준다.
	if(_InterlockedCompareExchange16(&mFireFlag, CAN_NOT_FIRE, CAN_FIRE) != CAN_FIRE)
	{
		return;
	}

	mbFiring = true;

	switch (mFireMode )
	{
		case EWaeponFireMode::SINGLE_FIRE:
			FireShot();
			if(mGameModeData.weapon != EWeaponType::SR)
			{
				GetWorld()->GetTimerManager().SetTimer(mFireDelayTimer, this, &ASRPlayerCharacter::endWeaponDelay, mFireDelay, false);
			}
			break;
		case EWaeponFireMode::BURST_FIRE:
			GetWorld()->GetTimerManager().SetTimer(mBurstFireTimer, this, &ASRPlayerCharacter::BurstFire, mFireDelay, true, 0.0f);
			break;
		case EWaeponFireMode::FULL_AUTO:
			GetWorld()->GetTimerManager().SetTimer(mFireDelayTimer, this, &ASRPlayerCharacter::FireShot, mFireDelay, true, 0.0f);
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("ASRPlayerCharacter- StartFire : 올바르지 않은 enum EWaeponFireMode 데이터입니다."));
			break;
	}
}

void ASRPlayerCharacter::StopFire()
{
	if(mFireMode == EWaeponFireMode::FULL_AUTO)
	{
		// flag를 끄고, 내부 타이머로 발사했으므로 점사가 끝나면 다시 flag를 켜준다.
		// SINGLE_FIRE의 timer 변수와 동일하므로 ClearTimer는 X.
		endWeaponDelay();
	}
	mRecoilFactor = 0.0f;
	mbFiring = false;
	mTutAnimInstance->SetFiring(false);
	mFirstShot = true;
}

void ASRPlayerCharacter::BurstFire()
{
	if (mCurrentBurst < 3)
	{
		FireShot();
		mCurrentBurst++;
	}
	else // 총알이 1 이상으로 정상적으로 점사가 완료된 경우.
	{
		mCurrentBurst = 0;
		GetWorldTimerManager().ClearTimer(mBurstFireTimer);
		mTutAnimInstance->SetFiring(false);

		// flag를 끄고, 내부 타이머로 발사했으므로 점사가 끝나면 다시 flag를 켜준다.
		endWeaponDelay();
	}
}

void ASRPlayerCharacter::FireShot()
{
	--mRemainAmmo;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), mMuzzleParticles, mWeapon->GetSocketTransform(FName("S_Muzzle")).GetLocation(), mWeapon->GetSocketTransform(FName("S_Muzzle")).GetRotation().Rotator());

	mTutAnimInstance->SetFiring(true);

	// 준비 기간에는 전적 및 정확도를 기록하지 않습니다.
	if(mPlayerController->IsStartMainGame())
	{
		mPlayerController->GetPlayerState()->OnAddFireShots(1);
	}

	mTutAnimInstance->Fire();

	// 반동 계수입니다. 스프레이의 반지름에 곱해집니다.
	// 0.1~1.0 사이로, 총알이 연속으로 발사될수록 계수가 높아집니다.
	// 권총은 계수가 그대로 유지됩니다.

	if(mFirstShot)
	{
		mRecoilFactor = 0.0f;
		mFirstShot = false;
	}
	else
	{
		if(mFireMode == EWaeponFireMode::SINGLE_FIRE)
		{
			mRecoilFactor = 0.15f;
			if(mGameModeData.weapon == EWeaponType::SR)
			{
				mRecoilFactor = 2.0f;
			}
		}
		else
		{
			mRecoilFactor += 0.1f;
			mRecoilFactor = FMath::Clamp(mRecoilFactor, 0.0f, 1.0f);
		}
	}

	FVector MuzzleLocation;
	FVector EndTrace;
	FCollisionQueryParams QueryParams = FCollisionQueryParams(SCENE_QUERY_STAT(WeaponTrace), false, this);
	FHitResult Hit;
	FActorSpawnParameters rules;
	rules.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASRProjectile* projectile = nullptr;
	switch(mGameModeData.game)
	{
		case EGameType::Battlefield:
		{
			// 총알이 캐릭터 카메라에서 발사되고, 투사체 방식입니다. (시리즈마다 다른 구현입니다. 4, 2042 시리즈와 비슷합니다.)
			// 랜덤 스프레이 방식입니다.
			// 총알의 스프레이 범위를 계산합니다.
			float x = 0.0f;
			float y = 0.0f;
			if(mbIsAiming)
			{
				x = FMath::RandRange(0.0f, 1.0f * mRecoilFactor) * (FMath::Cos(FMath::FRandRange(0.0f, 2*PI * mRecoilFactor)));
				y = FMath::RandRange(0.0f, 1.0f * mRecoilFactor) * (FMath::Sin(FMath::FRandRange(0.0f, 2*PI * mRecoilFactor)));
			}
			else
			{
				x = FMath::RandRange(-5.0f * mRecoilFactor, 5.0f * mRecoilFactor);
				y = FMath::RandRange(-5.0f * mRecoilFactor, 5.0f * mRecoilFactor);

			}
			FRotator randSpread(y, x, 0.0f);
			MuzzleLocation = mFirstPersonCameraComponent->GetComponentLocation() + (mFirstPersonCameraComponent->GetForwardVector()*100.0f);
			projectile = GetWorld()->SpawnActor<ASRProjectile>(mProjectileClass, MuzzleLocation, GetControlRotation()+ randSpread, rules);
			break;
			//ASRHandGunBullet::StaticClass()
		}
		case EGameType::RainbowSix:
		{
			// 총알이 캐릭터 카메라에서 발사되고, 히트스캔 방식입니다.
			// 랜덤 스프레이 방식입니다.
			// 총알의 스프레이 범위를 계산합니다.
			// 히트 스캔 방식이므로 끝지점 위치를 계산합니다.
			float spreadRadius = mbIsAiming ? 0.05f * mRecoilFactor : 0.05f;
			FVector2D randSpread = FMath::RandPointInCircle(spreadRadius);
			FVector spread(randSpread.X, 0.0f, randSpread.Y);

			MuzzleLocation = mScope->GetChildComponent(0)->GetSocketLocation(FName("S_Aim"));
			EndTrace = MuzzleLocation+ (mFirstPersonCameraComponent->GetForwardVector() + spread) * 10000.0f;

			if (GetWorld()->LineTraceSingleByChannel(Hit, MuzzleLocation, EndTrace, ECC_EngineTraceChannel3, QueryParams))
			{
				projectile = GetWorld()->SpawnActor<ASRProjectile>(mProjectileClass, Hit.Location, GetControlRotation(), rules);
				projectile->SetLifeSpan(2.0f);
				if (mPlayerController->IsDebugging())
				{
					DrawDebugLine(GetWorld(), MuzzleLocation, Hit.ImpactPoint, FColor::Red, false, 15.0f);
				}
			}
			break;
		}
		case EGameType::Tarkov:
		{
			// 총알이 총구에서 나가는 방식입니다.
			MuzzleLocation = mWeapon->GetSocketTransform(FName("S_Muzzle")).GetLocation();
			FRotator MuzzleRotation =mWeapon->GetSocketTransform(FName("S_Muzzle")).Rotator();

			projectile = GetWorld()->SpawnActor<ASRProjectile>(mProjectileClass, MuzzleLocation, MuzzleRotation, rules);
			break;
		}
		default:
			UE_LOG(LogTemp, Warning, TEXT("ASRPlayerCharacter- FireShot : 올바르지 않은 enum gametype 데이터입니다."));
			break;
	}
	// R6S 방식에서 ray가 아무것도 충돌을 감지하지 못하면 projectile이 생성되지 않으므로
	// nullptr가 발생함으로 허공에 발사시에 대한 안전장치.
	if(projectile != nullptr)
	{
		projectile->SetStartLocation(MuzzleLocation);
		projectile->SetDebugMode(mPlayerController->IsDebugging());
		projectile->SetBulletType(mGameModeData.weapon);
		projectile->BindPlayerStateInfo(mPlayerController->GetPlayerState());
	}

	mPlayerController->GetIngameHUD()->UpdateAmmo(mRemainAmmo);

	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), mFireSound, GetActorLocation());


	if(mFireMode == EWaeponFireMode::SINGLE_FIRE)
	{
		mTutAnimInstance->SetFiring(false);
	}

	if(mRemainAmmo <= 0)
	{
		// 탄이 다 떨어지면 재장전을 해야하므로, flag들을 키는 타이머를 초기화 해야합니다.
		if(mFireMode == EWaeponFireMode::BURST_FIRE)
		{
			GetWorldTimerManager().ClearTimer(mBurstFireTimer);
		}
		else
		{
			GetWorldTimerManager().ClearTimer(mFireDelayTimer);
		}

		mFireFlag = CAN_NOT_FIRE;
		mbIsEmptyMag = true;

		mTutAnimInstance->SetFiring(false);
	}
	else
	{
		// 저격총은 볼트액션식이므로 해당 변수를 true로 변경 합니다.
		if (mGameModeData.weapon == EWeaponType::SR)
		{
			mbNeedBoltAction = true;
			mBehaviorFlag = CAN_NOT_BEHAVIOR;
			//GetWorld()->GetTimerManager().SetTimer(mFireDelayTimer, this, &ASRPlayerCharacter::endWeaponDelay, mFireDelay, false);

		}
	}
}

void ASRPlayerCharacter::SetAim()
{

	if(mBehaviorFlag == CAN_NOT_BEHAVIOR)
	{
		// 저격총을 골랐을때 토글 모드에서 줌 상태에서 사격을 했을때 볼트 액션을 해야하지만
		// 조준모드를 해제할 수 없어 볼트액션 동작도 할 수 없으므로 별도 처리.
		// (재장전은 문제없음.)
		if (mGameModeData.weapon == EWeaponType::SR && mbIsAiming && mAimingType == EAimingType::Toggle)
		{
			mbIsAiming = false;
			mTutAnimInstance->SetAiming(mbIsAiming);
			mFirstPersonCameraComponent->SetFieldOfView(90.0f);
		}

		return;
	}

	if (mAimingType == EAimingType::Toggle)
	{
		mbIsAiming = mbIsAiming ? false : true;
		mTutAnimInstance->SetAiming(mbIsAiming);
		ESlateVisibility crosshairVisibility = mbIsAiming ? ESlateVisibility::Hidden : ESlateVisibility::Visible;
		if (mGameModeData.game != EGameType::Tarkov && mGameModeData.weapon != EWeaponType::SR)
		{
			mCrossHair->SetVisibility(crosshairVisibility);
		}

		if(mbIsAiming)
		{
			if (mGameModeData.game != EGameType::Tarkov)
			{
				if (mGameModeData.scope == EScopeType::Scope2dot5X)
				{
					mFirstPersonCameraComponent->SetFieldOfView(55.0f);
				}
				else if (mGameModeData.scope == EScopeType::Scope6X)
				{
					mFirstPersonCameraComponent->SetFieldOfView(45.0f);
				}
			}
		}
		else 
		{
			if (mGameModeData.scope != EScopeType::Scope1X && mGameModeData.game != EGameType::Tarkov)
			{
				mFirstPersonCameraComponent->SetFieldOfView(90.0f);
			}
		}
	}
	else // hold 방식의 조준입니다.
	{
		mbIsAiming = true;
		mTutAnimInstance->SetAiming(true);
		mCrossHair->SetVisibility(ESlateVisibility::Hidden);

		if(mGameModeData.game != EGameType::Tarkov)
		{
			if (mGameModeData.scope == EScopeType::Scope2dot5X)
			{
				mFirstPersonCameraComponent->SetFieldOfView(55.0f);
			}
			else if (mGameModeData.scope == EScopeType::Scope6X)
			{
				mFirstPersonCameraComponent->SetFieldOfView(45.0f);
			}
		}
	}
}

void ASRPlayerCharacter::SetHip()
{
	// hold의 경우. toggle방식은 SetAim함수에서 전부 처리되었습니다.
	if (mAimingType == EAimingType::Hold)
	{
		mbIsAiming = false;
		mTutAnimInstance->SetAiming(false);
		mFirstPersonCameraComponent->SetFieldOfView(90.0f);

		if(mGameModeData.game != EGameType::Tarkov && mGameModeData.weapon != EWeaponType::SR)
		{
			mCrossHair->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

// idle을 제외한 현재 동작이 끝나고 다른동작을 할 수 있도록 flag를 CAN_BEHAVIOR로 설정하는 함수입니다.
void ASRPlayerCharacter::endBehaviorDelay()
{
	mBehaviorFlag = CAN_BEHAVIOR;
}

// 재장전, 볼트액션 동작 후, 총을 발사할 수 있도록 flag를 CAN_FIRE로 설정하는 함수입니다.
void ASRPlayerCharacter::endWeaponDelay()
{
	mFireFlag = CAN_FIRE;
	GetWorld()->GetTimerManager().ClearTimer(mFireDelayTimer);
}

void ASRPlayerCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASRPlayerCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ASRPlayerCharacter::TurnAtRate(float Rate)
{
	if(mbIsAiming)
	{
		switch (mGameModeData.scope)
		{
			case EScopeType::Scope1X:
			{
				AddControllerYawInput(Rate * MouseSetting.Scope1X_X * GetWorld()->GetDeltaSeconds());
				break;
			}
			case EScopeType::Scope2dot5X:
			{
				AddControllerYawInput(Rate * MouseSetting.Scope2dot5X_X * GetWorld()->GetDeltaSeconds());
				break;
			}
			case EScopeType::Scope6X:
			{
				AddControllerYawInput(Rate * MouseSetting.Scope6X_X * GetWorld()->GetDeltaSeconds());
				break;
			}
			default:
				UE_LOG(LogTemp, Warning, TEXT("ASRPlayerCharacter- TurnAtRate : 올바르지 않은 enum EScopeType 데이터입니다."));
				break;
		}
	}
	else
	{
		AddControllerYawInput(Rate * MouseSetting.HipX * GetWorld()->GetDeltaSeconds());
	}
}

void ASRPlayerCharacter::LookUpAtRate(float Rate)
{

	if (mbIsAiming)
	{
		switch(mGameModeData.scope)
		{
			case EScopeType::Scope1X:
			{
				AddControllerPitchInput(Rate * MouseSetting.Scope1X_Y * GetWorld()->GetDeltaSeconds());
				break;
			}
			case EScopeType::Scope2dot5X:
			{
				AddControllerPitchInput(Rate * MouseSetting.Scope2dot5X_Y * GetWorld()->GetDeltaSeconds());
				break;
			}
			case EScopeType::Scope6X:
			{
				AddControllerPitchInput(Rate * MouseSetting.Scope6X_Y * GetWorld()->GetDeltaSeconds());
				break;
			}
			default:
				UE_LOG(LogTemp, Warning, TEXT("ASRPlayerCharacter- LookUpAtRate : 올바르지 않은 enum EScopeType 데이터입니다."));
				break;
		}
	}
	else
	{
		AddControllerPitchInput(Rate * MouseSetting.HipY * GetWorld()->GetDeltaSeconds());
	}
}


void ASRPlayerCharacter::SaveInGameSetting()
{
	auto settingData = Cast<USRInGameSetting>(UGameplayStatics::CreateSaveGameObject(USRInGameSetting::StaticClass()));
	settingData->MouseSensitivity = MouseSetting;
	settingData->AimingType = mAimingType;
	UGameplayStatics::SaveGameToSlot(settingData, settingData->GetSlotName(), settingData->GetSlotIndex());
}

void ASRPlayerCharacter::LoadInGameSetting()
{
	auto settingData = Cast<USRInGameSetting>(UGameplayStatics::CreateSaveGameObject(USRInGameSetting::StaticClass()));

	settingData = Cast<USRInGameSetting>(UGameplayStatics::LoadGameFromSlot(settingData->GetSlotName(), settingData->GetSlotIndex()));
	if(settingData)
	{
		MouseSetting = settingData->MouseSensitivity;
		mAimingType = settingData->AimingType;
	}
	else // 기존에 저장된 데이터가 없거나 불러올 수 없으면 기본값으로 초기화합니다.
	{
		FMouseSensitivity initMouseSetting;
		MouseSetting = initMouseSetting;
		mAimingType = EAimingType::Hold;
	}
}
