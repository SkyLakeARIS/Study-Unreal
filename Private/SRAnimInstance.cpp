// Fill out your copyright notice in the Description page of Project Settings.


#include "SRAnimInstance.h"
#include "SRPlayerController.h"
#include "Curves/CurveVector.h"
#include "Kismet/KismetMathLibrary.h"
#include "SRPlayerCharacter.h"

USRAnimInstance::USRAnimInstance()
{
	AimAlpha = 0.0f;
	bInterpAiming = false;
	bIsAiming = false;
	bInterpRelativeHand = false;
	ReloadAlpha = 1.0f;

	FireRate = 0.1f;
}

void USRAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

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

	if(!PlayerCharacter->GetNewWeapon())
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

	if(PlayerCharacter->IsCanFire())
	{
		SetLeftHandIK();
	}

	RecoilTick(DeltaSeconds);
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
	FTransform CamTransform = PlayerCharacter->GetFirstPersonCameraComponent()->GetComponentTransform();
	FTransform MeshTransform = PlayerCharacter->GetMesh1P()->GetComponentTransform();

	SightTransform = UKismetMathLibrary::MakeRelativeTransform(CamTransform, MeshTransform);

	float ADSdistance = 20.0f;
	if(PlayerCharacter->GetWeaponType() != EWeaponType::HG)
	{

		if (PlayerCharacter->GetGameType() == EGameType::Tarkov)
		{
			if (PlayerCharacter->GetScopeType() == EScopeType::Scope2dot5X)
			{
				ADSdistance = 5.0f;
			}
			else if (PlayerCharacter->GetScopeType() == EScopeType::Scope6X)
			{
				ADSdistance = 8.0f;
			}
		}
		else
		{
			if (PlayerCharacter->GetScopeType() == EScopeType::Scope1X)
			{
				ADSdistance = 15.0f;
			}
			else
			{
				ADSdistance = 10.0f;
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("ADSdistance :   %f"), ADSdistance);

	SightTransform.SetLocation(SightTransform.GetLocation() + SightTransform.GetRotation().Vector() * ADSdistance);
}

// 조준시 손의 위치를 계산
void USRAnimInstance::SetRelativeHandTransform()
{
	if (!PlayerCharacter->GetNewWeapon())
	{
		return;
	}

	FTransform OpticSocketTransform=FTransform();

	if(PlayerCharacter->GetNewScope()->GetChildActor()==nullptr)
	{
		OpticSocketTransform = PlayerCharacter->GetNewWeapon()->GetSocketTransform(FName("S_Optic"));
	}
	else
	{
		OpticSocketTransform = PlayerCharacter->GetNewScope()->GetChildComponent(0)->GetSocketTransform(FName("S_Aim"));
	}

	FTransform MeshTransform = PlayerCharacter->GetMesh1P()->GetSocketTransform(FName("hand_r"));

	RelativeHandTransform = UKismetMathLibrary::MakeRelativeTransform(OpticSocketTransform, MeshTransform);

}

void USRAnimInstance::SetFinalHandTransform()
{
	FTransform OpticSocketTransform = FTransform();

	if (!PlayerCharacter->GetNewScope())
	{
		OpticSocketTransform = PlayerCharacter->GetNewWeapon()->GetSocketTransform(FName("S_Optic"));
	}
	else
	{
		OpticSocketTransform = PlayerCharacter->GetNewScope()->GetChildComponent(0)->GetSocketTransform(FName("S_Aim"));
	}

	FTransform MeshTransform = PlayerCharacter->GetMesh1P()->GetSocketTransform(FName("ik_hand_root"));
	FinalHandTransform = UKismetMathLibrary::MakeRelativeTransform(OpticSocketTransform, MeshTransform);
}

void USRAnimInstance::SetLeftHandIK()
{
	if(!PlayerCharacter->GetNewWeapon())
	{
		return;
	}

	FTransform GunSocketTransform = PlayerCharacter->GetNewWeapon()->GetSocketTransform(FName("S_LeftHand"));
	FTransform MeshSocketTransform = PlayerCharacter->GetMesh1P()->GetSocketTransform(FName("hand_r"));

	// mesh(팔)을 총에 있는 그립 소켓에 맞는 위치를 계산해줌.
	LeftHandTransform = UKismetMathLibrary::MakeRelativeTransform(GunSocketTransform, MeshSocketTransform);

}

void USRAnimInstance::InterpAiming(float DeltaSeconds)
{
	AimAlpha = UKismetMathLibrary::FInterpTo(AimAlpha, static_cast<float>(bIsAiming), DeltaSeconds, 10.0f);

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
	if(VectorCurve)
	{
		FVector VelocityVec = PlayerCharacter->GetMovementComponent()->Velocity;
		VelocityVec.Z = 0.0f;
		float Velocity = VelocityVec.Size();
		float MaxSpeed = PlayerCharacter->GetMovementComponent()->GetMaxSpeed();
		Velocity = UKismetMathLibrary::NormalizeToRange(Velocity, (MaxSpeed/0.3f)*-1.0f, MaxSpeed);
		// 캐릭터가 이동시 만든 벡터 커브에 따라 총의 수직 흔들림을 구현.
		// 시간과 z의 곡선에 따라 현재 총위치와 보간하여 계산함.
		FVector NewVec = VectorCurve->GetVectorValue(PlayerCharacter->GetGameTimeSinceCreation());
		SwayLocation = UKismetMathLibrary::VInterpTo(SwayLocation, NewVec, DeltaSeconds, 1.8f)*Velocity;
		SwayLocation *= Velocity;
	}
	
}

// 화면을 움직일때 무기의 상하좌우 흔들림 구현 (타르코프)
void USRAnimInstance::RotateWithRotation(float DeltaSeconds)
{
	FRotator CurrentRotation = PlayerCharacter->GetControlRotation();
	TurnRotation = UKismetMathLibrary::RInterpTo(TurnRotation, CurrentRotation - OldRotation, DeltaSeconds, 3.0f);
	TurnRotation.Roll = TurnRotation.Pitch * -1.0;	// 화면을 상하로 움직일시 무기의 흔들림

	// 급격한 움직임에서 너무 큰 움직임을 보이지 않도록 제한함.
	TurnRotation.Yaw = FMath::Clamp(TurnRotation.Yaw, -7.0f, 7.0f);
	TurnRotation.Roll = FMath::Clamp(TurnRotation.Roll, -3.0f, 3.0f);

	TurnLocation.X = TurnRotation.Yaw/4.0f;
	TurnLocation.Z = TurnRotation.Roll/1.5f;

	OldRotation = CurrentRotation;
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

void USRAnimInstance::RecoilTimerFunction()
{
	bRecoil = false;
	GetWorld()->GetTimerManager().PauseTimer(FireTimer);
}

void USRAnimInstance::RecoveryStart()
{
	//if (mPlayerController->GetControlRotation().Pitch > RecoilStartRot.Pitch)
	//{
		bRecoilRecovery = true;
		GetWorld()->GetTimerManager().SetTimer(RecoveryTimer, this, &USRAnimInstance::RecoveryTimerFunction, RecoveryTime, false);
	//}
}

void USRAnimInstance::RecoveryTimerFunction()
{
	bRecoilRecovery = false;
}

void USRAnimInstance::SetAiming(bool IsAiming)
{
	if(bIsAiming != IsAiming)
	{
		bIsAiming = IsAiming;
		bInterpAiming = true;
	}
}

void USRAnimInstance::SetFiring(bool isFiring)
{
	bFiring = isFiring;
	if(bFiring)
	{
		RecoilStart();
	}
}


void USRAnimInstance::Reload()
{
	if(ReloadAlpha == 1.0f)
	{
		ReloadAlpha = 0.0f;
	}
	else if(ReloadAlpha ==0.0f)
	{
		ReloadAlpha = 1.0f;
	}
}

void USRAnimInstance::StopReload()
{
	ReloadAlpha = 1.0f;
}

void USRAnimInstance::Fire()
{
	FVector RecoilLoc;
	FRotator RecoilRot;
	bFiring = true;
	switch(PlayerCharacter->GetWeaponType())
	{
		case EWeaponType::AR:
		{
			if (bIsAiming)
			{
				RecoilLoc = FinalRecoilTransform.GetLocation();
				// 사격시 총이 뒤로 밀려나는 효과를 내는 벡터
				// x 좌우로 흔들림, y 뒤로 밀림
				RecoilLoc += FVector(FMath::RandRange(-0.3f, 0.3f), FMath::RandRange(-2.0f, -1.0f), FMath::RandRange(0.2f, 1.0f));

				RecoilRot = FinalRecoilTransform.GetRotation().Rotator();
				// p, y 좌우 반동, r = 수직반동 (-3은 안정적인 반동)
				RecoilRot += FRotator(FMath::RandRange(-5.0f, 5.0f), FMath::RandRange(-1.0f, 1.0f), -5.0f);
				FinalRecoilTransform.SetRotation(RecoilRot.Quaternion());
				FinalRecoilTransform.SetLocation(RecoilLoc);
			}
			else
			{
				RecoilLoc = FinalRecoilTransform.GetLocation();
				RecoilLoc += FVector(FMath::RandRange(-0.5f, 0.5f), FMath::RandRange(-5.0f, -1.0f), FMath::RandRange(0.2f, 1.0f));

				RecoilRot = FinalRecoilTransform.GetRotation().Rotator();
				RecoilRot += FRotator(FMath::RandRange(-10.0f, 10.0f), FMath::RandRange(-1.0f, 1.0f), -7.0f);
				FinalRecoilTransform.SetRotation(RecoilRot.Quaternion());
				FinalRecoilTransform.SetLocation(RecoilLoc);
			}
			break;
		}
		case EWeaponType::HG:
		{
			if (bIsAiming)
			{
				RecoilLoc = FinalRecoilTransform.GetLocation();
				RecoilLoc += FVector(0.0f, -1.0f, 0.2f);

				RecoilRot = FinalRecoilTransform.GetRotation().Rotator();
				RecoilRot += FRotator(FMath::RandRange(-2.0f, 2.0f), FMath::RandRange(-2.0f, 2.0f), -5.0f);
				FinalRecoilTransform.SetRotation(RecoilRot.Quaternion());
				FinalRecoilTransform.SetLocation(RecoilLoc);
			}
			else
			{
				RecoilLoc = FinalRecoilTransform.GetLocation();
				RecoilLoc += FVector(FMath::RandRange(-0.1f, 0.1f), FMath::RandRange(-2.0f, -3.f), FMath::RandRange(-2.0f, 2.0f));

				RecoilRot = FinalRecoilTransform.GetRotation().Rotator();
				RecoilRot += FRotator(FMath::RandRange(-3.0f, 3.0f), FMath::RandRange(-3.0f, 3.0f), -7.0f);
				FinalRecoilTransform.SetRotation(RecoilRot.Quaternion());
				FinalRecoilTransform.SetLocation(RecoilLoc);
			}
			break;
		}
		case EWeaponType::SR:
		{
			if (bIsAiming)
			{
				RecoilLoc = FinalRecoilTransform.GetLocation();
				RecoilLoc += FVector(5.0f, -5.0f, FMath::RandRange(0.2f, 1.0f));

				RecoilRot = FinalRecoilTransform.GetRotation().Rotator();
				RecoilRot += FRotator(-5.0f, FMath::RandRange(-1.0f, 1.0f), -5.0f);
				FinalRecoilTransform.SetRotation(RecoilRot.Quaternion());
				FinalRecoilTransform.SetLocation(RecoilLoc);
			}
			else
			{
				RecoilLoc = FinalRecoilTransform.GetLocation();
				RecoilLoc += FVector(5.0f, FMath::RandRange(-9.0f, -10.0f), FMath::RandRange(0.2f, 1.0f));

				RecoilRot = FinalRecoilTransform.GetRotation().Rotator();
				RecoilRot += FRotator(-5.0f, FMath::RandRange(-1.0f, 1.0f), -15.0f);
				FinalRecoilTransform.SetRotation(RecoilRot.Quaternion());
				FinalRecoilTransform.SetLocation(RecoilLoc);
			}
			break;
		}
	}
}

void USRAnimInstance::RecoilStart()
{

	PlayerDeltaRot = FRotator(0.0f, 0.0f, 0.0f);
	RecoilDeltaRot = FRotator(0.0f, 0.0f, 0.0f);
	Del = FRotator(0.0f, 0.0f, 0.0f);

	RecoilStartRot = UGameplayStatics::GetPlayerController(this, 0)->GetControlRotation();

	oldRotation = mPlayerController->GetControlRotation();
	bFiring = true;

	//Timer for the recoil: I have set it to 10s but dependeding how long it takes to empty the gun mag, you can increase the time.
	GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &USRAnimInstance::RecoilTimerFunction, 10.0f, false);

	bRecoil = true;
	bRecoilRecovery = false;
}

void USRAnimInstance::RecoilTick(float DeltaTime)
{
	if(PlayerCharacter->GetGameType() == EGameType::Tarkov)
	{
		return;
	}

	float recoiltime;
	FVector RecoilVec;
	FRotator originalRotator = mPlayerController->GetControlRotation();
	if (bRecoil)
	{

		//Calculation of control rotation to update 
		recoiltime = GetWorld()->GetTimerManager().GetTimerElapsed(FireTimer);
		RecoilVec = RecoilCurve->GetVectorValue(recoiltime);
		const float PitchLimit = 20.0f;
		Del.Roll = 0;

		if(PlayerCharacter->GetGameType() == EGameType::Battlefield)
		{
			if(PlayerCharacter->IsAimimg())
			{
				if (sumRecoil < PitchLimit)
				{
					Del.Pitch = 0.1f;
					sumRecoil += 0.1f;
				}
				else
				{
					Del.Pitch = 0.0f;
				}
				Del.Yaw = FMath::RandRange(-0.08f, 0.10f);
			}
			else
			{
				if(sumRecoil < PitchLimit)
				{
					Del.Pitch = 0.13f;
					sumRecoil += 0.13f;
				}
				else
				{
					Del.Pitch = 0.0f;
				}
				Del.Yaw = FMath::RandRange(-0.14f, 0.18f);	
			}
		}
		else //  EGameType::RainbowSix
		{
			if(PlayerCharacter->IsAimimg())
			{
				if (sumRecoil < PitchLimit)
				{
					Del.Pitch = 0.06f;
					sumRecoil += 0.06f;
				}
				else
				{
					Del.Pitch = 0.0f;
				}
				Del.Yaw = FMath::RandRange(-0.08f, 0.12f);
			}
			else
			{
				if (sumRecoil < PitchLimit)
				{
					Del.Pitch = 0.09f;
					sumRecoil += 0.09f;
				}
				else
				{
					Del.Pitch = 0.0f;
				}
				Del.Yaw = FMath::RandRange(-0.14f, 0.20f);
			}

		}

		PlayerDeltaRot = originalRotator - RecoilStartRot - RecoilDeltaRot;

		mPlayerController->SetControlRotation(UKismetMathLibrary::RInterpTo(originalRotator, originalRotator + Del, DeltaTime, 5000.0f));

		RecoilDeltaRot += Del;

		//Conditionally start resetting the recoil
		if (!bFiring)
		{
			if (recoiltime > FireRate)
			{
				GetWorld()->GetTimerManager().ClearTimer(FireTimer);
				RecoilStop();
				bRecoil = false;
				RecoveryStart();
			}
		}
	}
	else if (bRecoilRecovery)
	{
		FRotator tmprot = mPlayerController->GetControlRotation();

		mPlayerController->SetControlRotation(UKismetMathLibrary::RInterpTo(originalRotator, originalRotator - RecoilDeltaRot, DeltaTime, 5.0f));
		sumRecoil = UKismetMathLibrary::FInterpTo(sumRecoil, 0.0f, DeltaTime, 5.0f);
		RecoilDeltaRot = RecoilDeltaRot + (mPlayerController->GetControlRotation() - tmprot);
	}
}

void USRAnimInstance::RecoilStop()
{
	bFiring = false;
}
