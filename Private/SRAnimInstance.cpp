// Fill out your copyright notice in the Description page of Project Settings.
#include "SRAnimInstance.h"
#include "SRPlayerController.h"
#include "Curves/CurveVector.h"
#include "Kismet/KismetMathLibrary.h"
#include "SRPlayerCharacter.h"
#include "SRWeapon.h"

USRAnimInstance::USRAnimInstance()
{
	AimAlpha = 0.0f;
	bInterpAiming = false;
	bInterpRelativeHand = false;
	ReloadAlpha = 1.0f;
	bUseMultiThreadedAnimationUpdate = false;

	mRecoveryTime = 1.0f;
	mSumRecoil = 0.0f;
	mSumHorizonRecoil = 0.0f;
	mFireRate = 0.1f;
	mAimingAlpha = 0.0f;
}

void USRAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	enum { SEED = 184654 };
	mRandomStream.Initialize(SEED);
}

void USRAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(!PlayerCharacter)
	{
		return;
	}

	if (!mPlayerController)
	{
		return;
	}


	
	if(!PlayerCharacter->GetWeapon())
	{
		return;
	}

	if(bInterpAiming)
	{
		InterpAiming(DeltaSeconds);
	}

	if(bInterpRelativeHand)
	{
		InterpRelativeHand(DeltaSeconds);
	}

	RotateWithRotation(DeltaSeconds);
	
	MoveVectorCurve(DeltaSeconds);

	// 발사할 때만 실행
	if(!RecoilTransform.Equals(FTransform()) || !FinalRecoilTransform.Equals(FTransform()))
	{
		InterpRecoil(DeltaSeconds);
		InterpFinalRecoil(DeltaSeconds);
	}

	if(PlayerCharacter->GetBehaviorFlag() & (FIRING | AIMING))
	{
		SetLeftHandIK();
	}

	if (PlayerCharacter->GetGameType() != EGameType::Tarkov)
	{
		RecoilTick(DeltaSeconds);
	}
}

void USRAnimInstance::SetAim()
{
	bInterpAiming = true;
	mAimingAlpha = 1.0f;
}

void USRAnimInstance::UnsetAim()
{
	bInterpAiming = true;
	mAimingAlpha = 0.0f;
}

void USRAnimInstance::SetFire()
{
	RecoilStart();
}

void USRAnimInstance::UnsetFire()
{
	RecoilStop();
}

void USRAnimInstance::UpdateSocketInfo()
{
	PlayerCharacter = Cast<ASRPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	mPlayerController = Cast<ASRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	SetSightTransform();
	SetRelativeHandTransform();

	OldRotation = PlayerCharacter->GetControlRotation();
}

// 조준시 부착물이 위치할 위치를 계산
void USRAnimInstance::SetSightTransform()
{
	const FTransform camTransform = PlayerCharacter->GetFirstPersonCameraComponent()->GetComponentTransform();
	const FTransform meshTransform = PlayerCharacter->GetMesh1P()->GetComponentTransform();

	SightTransform = UKismetMathLibrary::MakeRelativeTransform(camTransform, meshTransform);

	float adsDistance = 20.0f;
	if(PlayerCharacter->GetWeaponType() != EWeaponType::HG)
	{

		if (PlayerCharacter->GetGameType() == EGameType::Tarkov)
		{
			if (PlayerCharacter->GetScopeType() == EScopeType::Scope2dot5X)
			{
				adsDistance = 5.0f;
			}
			else if (PlayerCharacter->GetScopeType() == EScopeType::Scope6X)
			{
				adsDistance = 8.0f;
			}
		}
		else
		{
			if (PlayerCharacter->GetScopeType() == EScopeType::Scope1X)
			{
				adsDistance = 15.0f;
			}
			else
			{
				adsDistance = 10.0f;
			}
		}
	}

	SightTransform.SetLocation(SightTransform.GetLocation() + SightTransform.GetRotation().Vector() * adsDistance);
}

// 조준시 손의 위치를 계산
void USRAnimInstance::SetRelativeHandTransform()
{
	if (!PlayerCharacter->GetWeapon())
	{
		return;
	}

	FTransform opticSocketTransform = FTransform();

	if(PlayerCharacter->GetWeapon()->GetScope()->GetChildActor()==nullptr)
	{
		opticSocketTransform = PlayerCharacter->GetWeapon()->GetGun()->GetSocketTransform(FName("S_Optic"));
	}
	else
	{
		opticSocketTransform = PlayerCharacter->GetWeapon()->GetScope()->GetChildComponent(0)->GetSocketTransform(FName("S_Aim"));
	}

	const FTransform meshTransform = PlayerCharacter->GetMesh1P()->GetSocketTransform(FName("hand_r"));
	

	RelativeHandTransform = UKismetMathLibrary::MakeRelativeTransform(opticSocketTransform, meshTransform);
}

void USRAnimInstance::SetFinalHandTransform()
{
	FTransform opticSocketTransform = FTransform();

	if (!PlayerCharacter->GetWeapon()->GetScope())
	{
		opticSocketTransform = PlayerCharacter->GetWeapon()->GetGun()->GetSocketTransform(FName("S_Optic"));
	}
	else
	{
		opticSocketTransform = PlayerCharacter->GetWeapon()->GetScope()->GetChildComponent(0)->GetSocketTransform(FName("S_Aim"));
	}
	//ik_hand_root (BoneName="ik_hand_r") ik_hand
	const FTransform meshTransform = PlayerCharacter->GetMesh1P()->GetSocketTransform(FName("ik_hand_root"));
	FinalHandTransform = UKismetMathLibrary::MakeRelativeTransform(opticSocketTransform, meshTransform);
}

void USRAnimInstance::SetLeftHandIK()
{
	if(!PlayerCharacter->GetWeapon())
	{
		return;
	}

	const FTransform gunSocketTransform = PlayerCharacter->GetWeapon()->GetGun()->GetSocketTransform(FName("S_LeftHand"));
	const FTransform meshSocketTransform = PlayerCharacter->GetMesh1P()->GetSocketTransform(FName("hand_r"));
	
	// mesh(팔)을 총에 있는 그립 소켓에 맞는 위치를 계산해줌.
	LeftHandTransform = UKismetMathLibrary::MakeRelativeTransform(gunSocketTransform, meshSocketTransform);
}

void USRAnimInstance::InterpAiming(float DeltaSeconds)
{
	AimAlpha = UKismetMathLibrary::FInterpTo(AimAlpha, mAimingAlpha, DeltaSeconds, 10.0f);

	if(AimAlpha >= 1.0f || AimAlpha <= 0.0f)
	{
		bInterpAiming = false;
	}
}

void USRAnimInstance::InterpRelativeHand(float DeltaSeconds)
{
	RelativeHandTransform = UKismetMathLibrary::TInterpTo(RelativeHandTransform, FinalHandTransform, DeltaSeconds, 10.0f);
	// 기존 부착물 위치에서 다른 부착물 위치로 손을 옮길때 보간하는데 보간이 끝나면(equals) false로.
	if(RelativeHandTransform.Equals(FinalHandTransform))
	{
		bInterpRelativeHand = false;
	}
}

void USRAnimInstance::MoveVectorCurve(float DeltaSeconds)
{
	FVector velocityVec = PlayerCharacter->GetMovementComponent()->Velocity;
	velocityVec.Z = 0.0f;
	float velocity = velocityVec.Size();
	const float maxSpeed = PlayerCharacter->GetMovementComponent()->GetMaxSpeed();
	velocity = UKismetMathLibrary::NormalizeToRange(velocity, (maxSpeed /0.3f)*-1.0f, maxSpeed);
	// 캐릭터가 이동시 만든 벡터 커브에 따라 총의 수직 흔들림을 구현.
	// 시간과 z의 곡선에 따라 현재 총위치와 보간하여 계산함.
	const FVector newVec = VectorCurve->GetVectorValue(PlayerCharacter->GetGameTimeSinceCreation());
	SwayLocation = UKismetMathLibrary::VInterpTo(SwayLocation, newVec, DeltaSeconds, 1.8f)* velocity;
	SwayLocation *= velocity;
}

// 화면을 움직일때 무기의 상하좌우 흔들림 구현 (타르코프)
void USRAnimInstance::RotateWithRotation(float DeltaSeconds)
{
	const FRotator currentRotation = PlayerCharacter->GetControlRotation();
	TurnRotation = UKismetMathLibrary::RInterpTo(TurnRotation, currentRotation - OldRotation, DeltaSeconds, 3.0f);
	TurnRotation.Roll = TurnRotation.Pitch * -1.0;	// 화면을 상하로 움직일시 무기의 흔들림

	// 급격한 움직임에서 너무 큰 움직임을 보이지 않도록 제한함.
	TurnRotation.Yaw = FMath::Clamp(TurnRotation.Yaw, -7.0f, 7.0f);
	TurnRotation.Roll = FMath::Clamp(TurnRotation.Roll, -3.0f, 3.0f);

	TurnLocation.X = TurnRotation.Yaw/4.0f;
	TurnLocation.Z = TurnRotation.Roll/1.5f;

	OldRotation = currentRotation;
}

void USRAnimInstance::InterpFinalRecoil(float DeltaSeconds)
{
	// interp to zero
	FinalRecoilTransform = UKismetMathLibrary::TInterpTo(FinalRecoilTransform, FTransform(), DeltaSeconds, 10.0f);
}

void USRAnimInstance::InterpRecoil(float DeltaSeconds)
{
	//interp to finalrecoiltransform
	RecoilTransform =  UKismetMathLibrary::TInterpTo(RecoilTransform, FinalRecoilTransform, DeltaSeconds, 10.0f);
}

void USRAnimInstance::recoilTimerFunction()
{
	mbRecoil = false;
	GetWorld()->GetTimerManager().PauseTimer(mFireTimer);
}

void USRAnimInstance::recoveryStart()
{

	if (mPlayerController->GetControlRotation().Pitch >= mRecoilStartRotator.Pitch)
	{
		mbRecoilRecovery = true;
		GetWorld()->GetTimerManager().SetTimer(mRecoveryTimer, this, &USRAnimInstance::recoveryTimerFunction, mRecoveryTime, false);
	}

}

void USRAnimInstance::recoveryTimerFunction()
{
	mbRecoilRecovery = false;
}

void USRAnimInstance::calcRecoilFactor(EGameType gameType)
{
	const float PitchLimit = (gameType == EGameType::Battlefield) ? 13.0f : 20.0f;
	const float YawLimit = 10.0f;

	float pitchFactor = 0.0f;
	float leftRecoil = 0.0f;
	float rightRecoil = 0.0f;

	if (PlayerCharacter->GetBehaviorFlag() & AIMING)
	{
		pitchFactor = (gameType == EGameType::Battlefield) ? 0.04f : 0.15f;
		leftRecoil = (gameType == EGameType::Battlefield) ? -0.07f : -0.08f;
		rightRecoil = (gameType == EGameType::Battlefield) ? 0.10f : 0.12f;

	}
	else
	{
		pitchFactor = (gameType == EGameType::Battlefield) ? 0.04f : 0.12f;
		leftRecoil = (gameType == EGameType::Battlefield) ? -0.07f : -0.14f;
		rightRecoil = (gameType == EGameType::Battlefield) ? 0.20f : 0.20f;
	}

	mDeltaRotator.Pitch = (mSumRecoil < PitchLimit) ? pitchFactor : 0.0f;

	mDeltaRotator.Yaw = (mRandomStream.FRandRange(leftRecoil, rightRecoil) >= 0.0f) ? rightRecoil : leftRecoil;
	// 좌우반동 Limit을 넘었어도 왼쪽 반동이면 허용합니다.
	if (mSumHorizonRecoil > YawLimit)
	{
		mDeltaRotator.Yaw = (mDeltaRotator.Yaw <= 0.0f) ? mDeltaRotator.Yaw : 0.0f;
	}

	mSumHorizonRecoil += mDeltaRotator.Yaw;
	mSumRecoil += mDeltaRotator.Pitch;
}

void USRAnimInstance::Fire()
{
	FVector recoilLoc = FinalRecoilTransform.GetLocation();
	FRotator recoilRot = FinalRecoilTransform.GetRotation().Rotator();

	switch(PlayerCharacter->GetWeaponType())
	{
		case EWeaponType::AR:
		{
			
			if (PlayerCharacter->GetBehaviorFlag() & AIMING)
			{
				// 사격시 총이 뒤로 밀려나는 효과를 내는 벡터
				// x 좌우로 흔들림, y 뒤로 밀림
				recoilLoc += FVector(FMath::RandRange(-0.3f, 0.3f), FMath::RandRange(-2.0f, -1.0f), FMath::RandRange(0.2f, 1.0f));

				// p, y 좌우 반동, r = 수직반동 (-3은 안정적인 반동)
				recoilRot += FRotator(FMath::RandRange(-5.0f, 5.0f), FMath::RandRange(-1.0f, 1.0f), -5.0f);
			}
			else
			{
				recoilLoc += FVector(FMath::RandRange(-0.5f, 0.5f), FMath::RandRange(-5.0f, -1.0f), FMath::RandRange(0.2f, 1.0f));
				recoilRot += FRotator(FMath::RandRange(-10.0f, 10.0f), FMath::RandRange(-1.0f, 1.0f), -7.0f);
			}
			break;
		}
		case EWeaponType::HG:
		{
			if (PlayerCharacter->GetBehaviorFlag() & AIMING)
			{
				recoilLoc += FVector(0.0f, -1.0f, 0.2f);
				recoilRot += FRotator(FMath::RandRange(-2.0f, 2.0f), FMath::RandRange(-2.0f, 2.0f), -5.0f);
			}
			else
			{
				recoilLoc += FVector(FMath::RandRange(-0.1f, 0.1f), FMath::RandRange(-2.0f, -3.f), FMath::RandRange(-2.0f, 2.0f));
				recoilRot += FRotator(FMath::RandRange(-3.0f, 3.0f), FMath::RandRange(-3.0f, 3.0f), -7.0f);
			}
			break;
		}
		case EWeaponType::SR:
		{
			if (PlayerCharacter->GetBehaviorFlag() & AIMING)
			{
				recoilLoc += FVector(5.0f, -5.0f, FMath::RandRange(0.2f, 1.0f));
				recoilRot += FRotator(-5.0f, FMath::RandRange(-1.0f, 1.0f), -5.0f);
			}
			else
			{
				recoilLoc += FVector(5.0f, FMath::RandRange(-9.0f, -10.0f), FMath::RandRange(0.2f, 1.0f));
				recoilRot += FRotator(-5.0f, FMath::RandRange(-1.0f, 1.0f), -15.0f);
			}
			break;
		}
		default:
			checkf(false, TEXT("USRAnimInstance-Fire 잘못된 EWeaponType입니다."));
			break;
	}
	FinalRecoilTransform.SetRotation(recoilRot.Quaternion());
	FinalRecoilTransform.SetLocation(recoilLoc);
}

void USRAnimInstance::RecoilStart()
{
	mPlayerDeltaRotator = FRotator::ZeroRotator;
	mRecoilDeltaRotator = FRotator::ZeroRotator;
	mDeltaRotator = FRotator::ZeroRotator;
	mRecoilStartRotator = mPlayerController->GetControlRotation();

	mbFiring = true;

	//Timer for the recoil: I have set it to 10s but dependeding how long it takes to empty the gun mag, you can increase the time.

	GetWorld()->GetTimerManager().SetTimer(mFireTimer, this, &USRAnimInstance::recoilTimerFunction, 10.0f, false);

	mbRecoil = true;
	mbRecoilRecovery = false;
}

void USRAnimInstance::RecoilTick(float DeltaTime)
{
	const EGameType gameType = PlayerCharacter->GetGameType();
	if (gameType == EGameType::Tarkov)
	{
		return;
	}

	const FRotator currentRotation = mPlayerController->GetControlRotation();
	if (mbRecoil)
	{

		calcRecoilFactor(gameType);

		mPlayerDeltaRotator = currentRotation - mRecoilStartRotator - mRecoilDeltaRotator;

		mPlayerController->SetControlRotation(UKismetMathLibrary::RInterpTo(currentRotation, currentRotation + mDeltaRotator, DeltaTime, 3000.0f));

		mRecoilDeltaRotator += mDeltaRotator;

		//Conditionally start resetting the recoil
		if (!mbFiring)
		{
			float recoilTime = GetWorld()->GetTimerManager().GetTimerElapsed(mFireTimer);

			if (recoilTime > mFireRate)
			{
				GetWorld()->GetTimerManager().ClearTimer(mFireTimer);
				RecoilStop();
				mbRecoil = false;
				recoveryStart();
			}
		}
	}
	else if (mbRecoilRecovery)
	{

		if(currentRotation.Pitch > mRecoilStartRotator.Pitch)
		{
			mPlayerController->SetControlRotation(UKismetMathLibrary::RInterpTo(currentRotation, currentRotation - mRecoilDeltaRotator, DeltaTime, 10.0f));
			mRecoilDeltaRotator = mRecoilDeltaRotator + (mPlayerController->GetControlRotation() - currentRotation);
		}
	}
}

void USRAnimInstance::RecoilStop()
{
	mSumRecoil = 0.0f;
	mSumHorizonRecoil = 0.0f;
	mbFiring = false;
	mRandomStream.Reset();
}
