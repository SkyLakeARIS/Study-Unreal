// Copyright Epic Games, Inc. All Rights Reserved.

#include "VersionCharacter.h"
#include "ADAnimInstance.h"
#include "VersionProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterPlayerController.h"
#include "DrawDebugHelpers.h"
#include "SRInGameSetting.h"
#include "SRWeapon.h"
#include "UIHUDWidget.h"
#include "UISelectModesWidget.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/KismetMathLibrary.h"
#include <string>


DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AVersionCharacter

AVersionCharacter::AVersionCharacter()
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

	// 무기관련
	FIRE_SWITCH_MODE = 3;
	FireModeOffset = 2;
	mRemainAmmo = 30;
	mARFireRate = 0.1f;
	CurrentBurst = 0;
	mCurrentFireMode  = EWaeponFireMode::FULL_AUTO;

	// 무기 상태 관련
	mbIsAiming = false;
	mbIsEmptyMag = false;
	mbFiring = false;
	mbNeedBoltAction = false;
	mbCanFire = true;
	mbIsReload = false;

	// 무기 반동
	mRecoilFactor = 0.0f;
	// 조준 방식
	mAimingType = EAimingType::Hold;

	// 소켓 위치
	mScopeLocationSocketName = FString("S_Scope1X");
	mWeaponLocationSocketName = FString("S_HandR_Rifle");
}

void AVersionCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void AVersionCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AVersionCharacter::SetAimToggleOrHold(EAimingType newType)
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
void AVersionCharacter::SniperMoveSocket(bool active)
{
	if(mGameModeData.weapon != EWeaponType::SR)
	{
		return;
	}


	if(active)
	{
		mWeapon->AttachToComponent(mMesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("ik_hand"));
		//atomic_thread_fence(std::memory_order_seq_cst);
		//mbCanFire = false;

		//atomic_thread_fence(std::memory_order_seq_cst);
		UE_LOG(LogTemp, Warning, TEXT("SniperMoveSocket change to new "));

	}
	else
	{
		mWeapon->AttachToComponent(mMesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), *mWeaponLocationSocketName);
		//atomic_thread_fence(std::memory_order_seq_cst);
		//mbCanFire = true;
		//atomic_thread_fence(std::memory_order_seq_cst);
		//mbNeedBoltAction = false;
		//atomic_thread_fence(std::memory_order_seq_cst);
	UE_LOG(LogTemp, Warning, TEXT("SniperMoveSocket change to original"));

	}
}

// 볼트 액션이 동작이 끝나고 다시 사격이 가능해지면 호출됩니다. SniperAnimInstacne에서 호출됩니다.
void AVersionCharacter::NotifyBoltActionEnd()
{
	mbCanFire = true;
	//atomic_thread_fence(std::memory_order_seq_cst);
	mbNeedBoltAction = false;
	atomic_thread_fence(std::memory_order_seq_cst);
}

// 재장전 액션이 동작이 끝나고 다시 사격이 가능해지면 호출됩니다. 캐릭터AnimInstacne들에서 호출됩니다.
void AVersionCharacter::NotifyReloadEnd()
{
	mRemainAmmo = mMaxMagAmount;
	mPlayerController->GetIngameHUD()->UpdateAmmo(mRemainAmmo);

	mbCanFire = true;
	atomic_thread_fence(std::memory_order_seq_cst);

	mbNeedBoltAction = false;
	atomic_thread_fence(std::memory_order_seq_cst);

	mbIsEmptyMag = false;
	mbIsReload = false;
}


void AVersionCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	mPlayerController = Cast<ACharacterPlayerController>(GetController());
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
	GetWorld()->GetTimerManager().SetTimer(bindTimerHandle, this, &AVersionCharacter::tryBindSelectModesUI, 1.0f, false, 1.0f);
	FTimerHandle loadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(loadTimerHandle, this, &AVersionCharacter::LoadInGameSetting, 1.0f, false, 1.0f);
}

void AVersionCharacter::tryBindSelectModesUI()
{
	mPlayerController->GetSelectModesWidget()->BindCharacterInfo(this);
}

// 타이머에 의해 호출되는 히트마크를 지우는 함수입니다.
void AVersionCharacter::clearHitMark()
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

void AVersionCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AVersionCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AVersionCharacter::StopFire);
	PlayerInputComponent->BindAction("SwitchFireMode", IE_Pressed, this, &AVersionCharacter::SwitchFireMode);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AVersionCharacter::Reload);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AVersionCharacter::SetAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AVersionCharacter::SetHip);

	//PlayerInputComponent->BindAxis("MoveForward", this, &AVersionCharacter::MoveForward);
	//PlayerInputComponent->BindAxis("MoveRight", this, &AVersionCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this,&AVersionCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AVersionCharacter::LookUpAtRate);
}

EWeaponType AVersionCharacter::GetWeaponType() const
{
	return mGameModeData.weapon;
}

EScopeType AVersionCharacter::GetScopeType() const
{
	return mGameModeData.scope;
}

EGameType AVersionCharacter::GetGameType() const
{
	return mGameModeData.game;
}

// 타겟에 적중시 적절한 히트마크를 크로스헤어가 위치하는 곳에 표시하는 함수입니다.
// 표시할 히트마크의 유형은 projectile 개체가 전달해줍니다.
void AVersionCharacter::AddViewPortHitMark(EHitType hitType)
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
			UE_LOG(LogTemp, Display, TEXT("AVersionCharacter - AddViewPortHitMark : 올바르지 않은 EHitType 타입입니다."));
			break;
	}
	GetWorld()->GetTimerManager().SetTimer(mHitMarkTimer, this, &AVersionCharacter::clearHitMark, 0.5f, false, -1.0f);
}

bool AVersionCharacter::IsAimimg() const
{
	atomic_thread_fence(std::memory_order_seq_cst);

	return mbIsAiming;
}

EAimingType AVersionCharacter::GetAimingType() const
{
	atomic_thread_fence(std::memory_order_seq_cst);
	return mAimingType;
}

bool AVersionCharacter::IsCanFire() const
{
	atomic_thread_fence(std::memory_order_seq_cst);

	return mbCanFire;
}

bool AVersionCharacter::IsNeedBoltAction() const
{
	atomic_thread_fence(std::memory_order_seq_cst);

	return mbNeedBoltAction;
}

bool AVersionCharacter::IsFire() const
{
	atomic_thread_fence(std::memory_order_seq_cst);

	return mbFiring;
}

bool AVersionCharacter::IsEmptyMag() const
{
	atomic_thread_fence(std::memory_order_seq_cst);

	return mbIsEmptyMag;
}

bool AVersionCharacter::IsReload() const
{
	atomic_thread_fence(std::memory_order_seq_cst);

	return mbIsReload;
}

/*
 *  맵 선택 UI에서 인게임으로 들어올 때 무기와(총, 조준경) 게임모드를 선택하는 UI로부터
 *	데이터를 넘겨받아 캐릭터 데이터를 업데이트하는 함수입니다.
 */
void AVersionCharacter::InitGameMode(FGameModeData modeData)
{
	mGameModeData = modeData;

	// 게임 모드 데이터를 초기화 합니다.
	FString DisplayMode;
	switch (mGameModeData.game)
	{
	case EGameType::Battlefield:
	{
		if(mGameModeData.weapon != EWeaponType::SR)
		{
			mCrossHair->SetVisibility(ESlateVisibility::Visible);
		}


		UE_LOG(LogTemp, Display, TEXT("InitGameMode : battlefield mode is applied"));
		DisplayMode = "battlefield mode";
		break;
	}
	case EGameType::RainbowSix:
	{
		if (mGameModeData.weapon != EWeaponType::SR)
		{
			mCrossHair->SetVisibility(ESlateVisibility::Visible);
		}

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
		UE_LOG(LogTemp, Warning, TEXT("AVersionCharacter - InitGameMode : 올바르지 않은 enum EGameType 데이터입니다."));
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
			mScope->GetChildActor()->SetActorRotation(mScope->GetChildActor()->GetActorRotation() + FRotator(-0.2f, 0.0f, 0.0f));
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

				mCurrentFireMode = EWaeponFireMode::FULL_AUTO;

				mSwitchFireModeSound = mWeaponData->GetSwtichFireModeSound();
				break;
			}
		case EWeaponType::HG:
			{
				mMaxMagAmount = EWeaponMagSize::HG;
				mRemainAmmo = mMaxMagAmount;

				mCurrentFireMode = EWaeponFireMode::SINGLE_FIRE;
				break;
			}
		case EWeaponType::SR:
			{
				mMaxMagAmount = EWeaponMagSize::SR;
				mRemainAmmo = mMaxMagAmount;

				mCurrentFireMode = EWaeponFireMode::SINGLE_FIRE;

				// 저격총의 경우 조준경의 크기가 비정상적으로 커지는 문제로 인해서 강제로 크기를 조정합니다.
				mScope->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
				break;
			}
		default:
			UE_LOG(LogTemp, Warning, TEXT("InitGameMode : InitGameMode - 올바르지 않은 enum EWeaponType 데이터입니다."));
			break;
	}



	// 초기화된 데이터들을 업데이트 합니다.
	mTutAnimInstance = Cast<UADAnimInstance>(mMesh1P->GetAnimInstance());
	mTutAnimInstance->UpdateSocketInfo();

	mPlayerController->GetIngameHUD()->UpdateGameMode(DisplayMode);
	mPlayerController->GetIngameHUD()->UpdateFireMode(mCurrentFireMode);
	mPlayerController->GetIngameHUD()->UpdateAmmo(mRemainAmmo);
}

// 재장전 함수. AnimNotify에 의해서 다시 사격가능한 상태가 됩니다.
void AVersionCharacter::Reload()
{
	atomic_thread_fence(std::memory_order_seq_cst);
	mbCanFire = false;
	atomic_thread_fence(std::memory_order_seq_cst);
	mbIsReload = true;
	atomic_thread_fence(std::memory_order_seq_cst);

}

/*
 *	무기의 발사 모드를 변경하는 함수.
 *  권총과 저격총의 경우 Single모드 하나만 존재하므로
 *	소총만 발사모드가 작동됨.
 */
void AVersionCharacter::SwitchFireMode()
{
	if(mGameModeData.weapon == EWeaponType::AR)
	{
		EWaeponFireMode modes[] = { EWaeponFireMode::SINGLE_FIRE, EWaeponFireMode::BURST_FIRE,EWaeponFireMode::FULL_AUTO };
		FireModeOffset = (FireModeOffset + 1) % FIRE_SWITCH_MODE;
		mCurrentFireMode  = modes[FireModeOffset];

		UGameplayStatics::PlaySoundAtLocation(this, mSwitchFireModeSound, GetActorLocation());
		mPlayerController->GetIngameHUD()->UpdateFireMode(mCurrentFireMode);
	}
}

void AVersionCharacter::StartFire()
{

		// Montage_GetIsStopped
		//;mTutAnimInstance->IsAnyMontagePlaying()
		
	
	atomic_thread_fence(std::memory_order_seq_cst);
	if (!mbCanFire)
	{
		return;
	}

	if (mRemainAmmo <= 0)
	{
		UGameplayStatics::PlaySoundAtLocation(this, mDryFireSound, GetActorLocation());
		return;
	}

	mbFiring = true;

	switch (mCurrentFireMode )
	{
		case EWaeponFireMode::SINGLE_FIRE:
			FireShot();
			break;
		case EWaeponFireMode::BURST_FIRE:
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_BurstMode, this, &AVersionCharacter::BurstFire, mARFireRate, true, 0.0f);
			break;
		case EWaeponFireMode::FULL_AUTO:
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_FullAutoMode, this, &AVersionCharacter::FireShot, mARFireRate, true, 0.0f);
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("AVersionCharacter- StartFire : 올바르지 않은 enum EWaeponFireMode 데이터입니다."));
			break;
	}
}

void AVersionCharacter::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_FullAutoMode);
	mRecoilFactor = 0.0f;
	mbFiring = false;
	mTutAnimInstance->SetFiring(false);
	mFirstShot = true;
}

void AVersionCharacter::BurstFire()
{
	if (CurrentBurst < 3)
	{
		FireShot();
		CurrentBurst++;
	}
	else
	{
		CurrentBurst = 0;
		GetWorldTimerManager().ClearTimer(TimerHandle_BurstMode);
	}
}

void AVersionCharacter::FireShot()
{
	--mRemainAmmo;
	if(mRemainAmmo < 0)
	{
		return;
	}

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), mMuzzleParticles, mWeapon->GetSocketTransform(FName("S_Muzzle")).GetLocation(), mWeapon->GetSocketTransform(FName("S_Muzzle")).GetRotation().Rotator());

	mTutAnimInstance->SetFiring(true);
	mPlayerController->GetPlayerState()->OnAddFireShots(1);
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
		if(mCurrentFireMode == EWaeponFireMode::SINGLE_FIRE)
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
	AVersionProjectile* projectile = nullptr;
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
			projectile = GetWorld()->SpawnActor<AVersionProjectile>(mProjectileClass, MuzzleLocation, GetControlRotation()+ randSpread, rules);
			break;
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

	//		float x = FMath::RandRange(0.0f, 1.0f) * FMath::Cos(FMath::FRandRange(0.0f, 0.0f));
	//		float y = FMath::RandRange(0.0f, 1.0f) * FMath::Exp(0.5) * FMath::Sin(FMath::FRandRange(0.0f, 0.0f));
	//		FVector randSpread(x, 0, y);
 //+ randSpread)
			MuzzleLocation = mScope->GetChildComponent(0)->GetSocketLocation(FName("S_Aim"));
			EndTrace = MuzzleLocation+ (mFirstPersonCameraComponent->GetForwardVector() + spread) * 10000.0f;

			if (GetWorld()->LineTraceSingleByChannel(Hit, MuzzleLocation, EndTrace, ECC_EngineTraceChannel3, QueryParams))
			{
				projectile = GetWorld()->SpawnActor<AVersionProjectile>(mProjectileClass, Hit.Location, GetControlRotation(), rules);
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

			projectile = GetWorld()->SpawnActor<AVersionProjectile>(mProjectileClass, MuzzleLocation, MuzzleRotation, rules);
			break;
		}
		default:
			UE_LOG(LogTemp, Warning, TEXT("AVersionCharacter- FireShot : 올바르지 않은 enum gametype 데이터입니다."));
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

	if(mRemainAmmo <= 0)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_BurstMode);
		mbIsEmptyMag = true;
		atomic_thread_fence(std::memory_order_seq_cst);
		//mbCanFire = false;
		//atomic_thread_fence(std::memory_order_seq_cst);
		mTutAnimInstance->SetFiring(false);
	}
	else
	{
		// 저격총은 볼트액션식이므로 해당 변수를 true로 변경 합니다.
		if (mGameModeData.weapon == EWeaponType::SR)
		{
			//mbCanFire = false;
		//	atomic_thread_fence(std::memory_order_seq_cst);
			mbNeedBoltAction = true;
			atomic_thread_fence(std::memory_order_seq_cst);

		//	FTimerHandle delay;
		//	GetWorld()->GetTimerManager().SetTimer(delay, this, &AVersionCharacter::boltactionDelay, 1.2f, false);
		}
	}
}

void AVersionCharacter::SetAim()
{
	if(mbIsReload)
	{
		return;
	}

	if (mAimingType == EAimingType::Toggle)
	{
		mbIsAiming = !mbIsAiming;
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

void AVersionCharacter::SetHip()
{
	// hold의 경우. toggle방식은 SetAim함수에서 전부 처리되었습니다.
	if (mAimingType == EAimingType::Hold)
	{
		mbIsAiming = false;
		mTutAnimInstance->SetAiming(false);

		if(mGameModeData.game != EGameType::Tarkov)
		{
			mCrossHair->SetVisibility(ESlateVisibility::Visible);

			if (mGameModeData.scope != EScopeType::Scope1X)
			{
				mFirstPersonCameraComponent->SetFieldOfView(90.0f);
			}
		}
	}
}

void AVersionCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AVersionCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AVersionCharacter::TurnAtRate(float Rate)
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
				UE_LOG(LogTemp, Warning, TEXT("AVersionCharacter- TurnAtRate : 올바르지 않은 enum EScopeType 데이터입니다."));
				break;
		}
	}
	else
	{
		AddControllerYawInput(Rate * MouseSetting.HipX * GetWorld()->GetDeltaSeconds());
	}
}

void AVersionCharacter::LookUpAtRate(float Rate)
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
				UE_LOG(LogTemp, Warning, TEXT("AVersionCharacter- LookUpAtRate : 올바르지 않은 enum EScopeType 데이터입니다."));
				break;
		}
	}
	else
	{
		AddControllerPitchInput(Rate * MouseSetting.HipY * GetWorld()->GetDeltaSeconds());
	}
}


void AVersionCharacter::SaveInGameSetting()
{
	auto settingData = Cast<USRInGameSetting>(UGameplayStatics::CreateSaveGameObject(USRInGameSetting::StaticClass()));
	settingData->MouseSensitivity = MouseSetting;
	settingData->AimingType = mAimingType;
	UGameplayStatics::SaveGameToSlot(settingData, settingData->GetSlotName(), settingData->GetSlotIndex());
}

void AVersionCharacter::LoadInGameSetting()
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
