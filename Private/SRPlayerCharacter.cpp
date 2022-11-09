// Copyright Epic Games, Inc. All Rights Reserved.
#include "SRPlayerCharacter.h"
#include "SRPlayerController.h"
#include "SRAnimInstance.h"
#include "Components/InputComponent.h"
#include "SRGameMode.h"
#include "SRWeapon.h"
#include "UIHUDWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

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

	mBehaviorFlag = CLEAR_STATE;
	// 조준 방식
	mAimingType = eAimingType::Hold;
}

void ASRPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	mOnCrossHairVisibility.Unbind();
}

void ASRPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

USRAnimInstance* ASRPlayerCharacter::GetAnimInstance() const
{
	return mAnimInstance;
}

void ASRPlayerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	mPlayerController = Cast<ASRPlayerController>(GetController());
	mWeapon = GetWorld()->SpawnActor<ASRWeapon>(ASRWeapon::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);

	EnableInput(mPlayerController);
}


void ASRPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASRPlayerCharacter::startFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASRPlayerCharacter::stopFire);
	PlayerInputComponent->BindAction("SwitchFireMode", IE_Pressed, this, &ASRPlayerCharacter::switchFireMode);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASRPlayerCharacter::reload);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ASRPlayerCharacter::setAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ASRPlayerCharacter::setHip);

	//PlayerInputComponent->BindAxis("MoveForward", this, &ASRPlayerCharacter::MoveForward);
	//PlayerInputComponent->BindAxis("MoveRight", this, &ASRPlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &ASRPlayerCharacter::turnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ASRPlayerCharacter::lookUpAtRate);
}

eWeaponType ASRPlayerCharacter::GetWeaponType() const
{
	return mWeaponType;
}

eScopeType ASRPlayerCharacter::GetScopeType() const
{
	return mScopeType;
}

eGameType ASRPlayerCharacter::GetGameType() const
{
	return mGameType;
}

ASRWeapon* ASRPlayerCharacter::GetWeapon() const
{
	return mWeapon;
}

eAimingType ASRPlayerCharacter::GetAimingType() const
{
	return mAimingType;
}

/*
 *  GameMode 의 select ui에서 세팅된 데이터들을 넘겨받아 캐릭터 데이터를 업데이트하는 함수입니다.
 */
void ASRPlayerCharacter::InitDataFromGameMode(const FGameModeData modeData)
{
	mGameType = modeData.game;
	mScopeType = modeData.scope;
	mWeaponType = modeData.weapon;

	auto* const gameMode = Cast<ASRGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	mOnCrossHairVisibility.BindUObject(gameMode->GetHUDWidget(), &UUIHUDWidget::SetCrosshairVisibility);

	if(mGameType != eGameType::Tarkov && mWeaponType != eWeaponType::SR)
	{
		mOnCrossHairVisibility.Execute(ESlateVisibility::Visible);
	}

	mWeapon->Initialize(mGameType, mScopeType, mWeaponType, *this);

	// 초기화된 데이터들을 업데이트 합니다.
	mAnimInstance = Cast<USRAnimInstance>(mMesh1P->GetAnimInstance());
	mAnimInstance->UpdateSocketInfo();

	mBehaviorFlag = CLEAR_STATE;
}

/*
 *  조준 방식을 변경합니다. (hold, toggle)
 */
void ASRPlayerCharacter::SetAimingType(const eAimingType newSetting)
{
	mAimingType = newSetting;
}

/*
 *  파라미터로 받아온 데이터로 마우스 감도를 변경합니다.
 */
void ASRPlayerCharacter::SetMouseSensitivity(const float hipYaw, const float hipPitch, const float aimYaw, const float aimPitch)
{
	mAimYawSensitivity = aimYaw;
	mAimPitchSensitivity = aimPitch;

	mHipYawSensitivity = hipYaw;
	mHipPitchSensitivity = hipPitch;
}


/*
 * 재장전 함수.
 * Anim blueprint에 의해서 behavior 프래그가 설정되고, 다시 사격가능한 상태가 됩니다.
 */ 
void ASRPlayerCharacter::reload()
{

	if(mBehaviorFlag & (AIMING | RELOADING | FULL_RELOADING))
	{
		return;
	}

	if(mBehaviorFlag & WAIT_TO_END_ANIMATION)
	{
		return;
	}

	UnsetBehaviorFlag(FIRING);

	SetBehaviorFlag(RELOADING);
	SetBehaviorFlag(WAIT_TO_END_ANIMATION);

	mWeapon->Reload();

	mFirstPersonCameraComponent->SetFieldOfView(90.0f);
	if(!(mWeaponType == eWeaponType::SR || mGameType == eGameType::Tarkov))
	{
		mOnCrossHairVisibility.Execute(ESlateVisibility::Visible);
	}

}

/*
 *	무기의 발사 모드를 변경하는 함수.
 *  권총과 저격총의 경우 SingleFire모드 하나만 존재하므로
 *	소총만 발사모드가 작동됨.
 */
void ASRPlayerCharacter::switchFireMode()
{
	mWeapon->SwitchFireMode();
}

void ASRPlayerCharacter::setToggleAim()
{

	if (mBehaviorFlag & AIMING)
	{
		UnsetBehaviorFlag(AIMING);
		mAnimInstance->UnsetAim();
		if (mScopeType != eScopeType::Scope1X && mGameType != eGameType::Tarkov)
		{
			mFirstPersonCameraComponent->SetFieldOfView(90.0f);
		}

	}
	else
	{
		SetBehaviorFlag(AIMING);
		mAnimInstance->SetAim();

		if (mGameType != eGameType::Tarkov)
		{
			if (mScopeType == eScopeType::Scope2dot5X)
			{
				mFirstPersonCameraComponent->SetFieldOfView(55.0f);
			}
			else if (mScopeType == eScopeType::Scope6X)
			{
				mFirstPersonCameraComponent->SetFieldOfView(45.0f);
			}
		}
	}

	const ESlateVisibility visibility = (mBehaviorFlag & AIMING) ? ESlateVisibility::Hidden : ESlateVisibility::Visible;
	if (mGameType != eGameType::Tarkov && mWeaponType != eWeaponType::SR)
	{
		mOnCrossHairVisibility.Execute(visibility);
	}
	
}

void ASRPlayerCharacter::startFire()
{

	if (mWeapon->GetRemainAmmo() <= 0)
	{
		return;
	}

	// 사격은 조준을 제외한 경우는 허용하지 않습니다.
	// WAIT_TO_END_ANIMATION flag도 포함될 줄 알았으나.
	// 따로 체크해야 정상적으로 체크가 된다. 생각 못한 부분이 있는지 고민이 필요.
	if(mBehaviorFlag & ~AIMING)
	{
		return;
	}

	if (mBehaviorFlag & WAIT_TO_END_ANIMATION)
	{
		return;
	}

	SetBehaviorFlag(FIRING);
	mAnimInstance->SetFire();
	mWeapon->StartFire();
}

void ASRPlayerCharacter::stopFire()
{
	// BurstFire 중이면 점사가 끝나지 않았으므로 무시합니다.
	if(mWeapon->GetFireMode() == eWaeponFireMode::BURST_FIRE && mWeapon->IsBurstShot())
	{
		return;
	}

	mAnimInstance->UnsetFire();
	mWeapon->StopFire();
	UnsetBehaviorFlag(FIRING);
}

void ASRPlayerCharacter::setAim()
{
	// 토글 모드시 줌 상태에서 탄이 비었을 때, 재장전 혹은 볼트 액션을 해야하지만
	// 조준모드를 해제할 수 없어 재장전, 볼트액션 동작을 할 수 없으므로 별도 처리.
	if ((mBehaviorFlag & AIMING) && (mBehaviorFlag & (BOLTACTION | FULL_RELOADING | RELOADING)))
	{
		if (mAimingType == eAimingType::Toggle)
		{
			setToggleAim();
		}
		return;
	}

	if (mBehaviorFlag & WAIT_TO_END_ANIMATION)
	{
		return;
	}

	// toggle 방식은 따로 처리합니다.
	if (mAimingType == eAimingType::Toggle)
	{
		setToggleAim();
		return;
	}

	SetBehaviorFlag(AIMING);
	mAnimInstance->SetAim();
	mOnCrossHairVisibility.Execute(ESlateVisibility::Hidden);

	if (mGameType != eGameType::Tarkov)
	{
		if (mScopeType == eScopeType::Scope2dot5X)
		{
			mFirstPersonCameraComponent->SetFieldOfView(55.0f);
		}
		else if (mScopeType == eScopeType::Scope6X)
		{
			mFirstPersonCameraComponent->SetFieldOfView(45.0f);
		}
	}
}

void ASRPlayerCharacter::setHip()
{
	// hold의 경우. toggle방식은 SetAim함수에서 전부 처리되었습니다.
	if (mAimingType != eAimingType::Hold)
	{
		return;
	}
	UnsetBehaviorFlag(AIMING);
	mAnimInstance->UnsetAim();

	mFirstPersonCameraComponent->SetFieldOfView(90.0f);

	if(mGameType != eGameType::Tarkov && mWeaponType != eWeaponType::SR)
	{
		mOnCrossHairVisibility.Execute(ESlateVisibility::Visible);
	}
}

void ASRPlayerCharacter::moveForward(const float value)
{
	if (value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), value);
	}
}

void ASRPlayerCharacter::moveRight(const float value)
{
	if (value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), value);
	}
}

void ASRPlayerCharacter::turnAtRate(const float rate)
{
	if(mBehaviorFlag & AIMING)
	{
		AddControllerYawInput(rate * mAimYawSensitivity * GetWorld()->GetDeltaSeconds());
	}
	else
	{
		AddControllerYawInput(rate * mHipYawSensitivity* GetWorld()->GetDeltaSeconds());
	}

	mPlayerController->CalcTargetIndicatorAndShow();
}

void ASRPlayerCharacter::lookUpAtRate(const float rate)
{
	if (mBehaviorFlag & AIMING)
	{
		AddControllerPitchInput(rate * mAimPitchSensitivity * GetWorld()->GetDeltaSeconds());
	}
	else
	{
		AddControllerPitchInput(rate * mHipPitchSensitivity * GetWorld()->GetDeltaSeconds());
	}
}


int32 ASRPlayerCharacter::GetBehaviorFlag() const
{
	return mBehaviorFlag;
}