// Fill out your copyright notice in the Description page of Project Settings.


#include "ADAnimInstance.h"

#include "Camera/CameraComponent.h"

#include "Curves/CurveVector.h"

#include "GameFramework/PawnMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"

#include "VersionCharacter.h"

UADAnimInstance::UADAnimInstance()
{
	AimAlpha = 0.0f;
	bInterpAiming = false;
	bIsAiming = false;
	bInterpRelativeHand = false;
	ReloadAlpha = 1.0f;
}

void UADAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	Character = Cast<AVersionCharacter>(TryGetPawnOwner());

	if(Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("Valid  Character"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("invalid  Character"));

	}

	if(Character)
	{
		FTimerHandle TSetSightTransform;
		FTimerHandle TSetRelativeHandTransform;

		// 너무 빠르게 초기화를하면 mesh들이(부착물) nullptr이 될 가능성이 있으므로 딜레이를 줌.
		GetWorld()->GetTimerManager().SetTimer(TSetSightTransform, this, &UADAnimInstance::SetSightTransform, 0.3f, false);
		GetWorld()->GetTimerManager().SetTimer(TSetRelativeHandTransform, this, &UADAnimInstance::SetRelativeHandTransform, 0.3f, false);

		OldRotation = Character->GetControlRotation();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get character"));
	}
}

void UADAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(!Character)
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


	SetLeftHandIK();
}

// 조준시 부착물이 위치할 위치를 계산
void UADAnimInstance::SetSightTransform()
{
	FTransform CamTransform = Character->GetFirstPersonCameraComponent()->GetComponentTransform();
	FTransform MeshTransform = Character->GetMesh1P()->GetComponentTransform();

	SightTransform = UKismetMathLibrary::MakeRelativeTransform(CamTransform, MeshTransform);

	SightTransform.SetLocation(SightTransform.GetLocation() + SightTransform.GetRotation().Vector() * 30.0f);
	//// 조준 했을때의 mesh(총+팔+부착물)위치를 구하려는 과정임.
	//FVector NewSightVector = Relative.GetLocation();

	//FVector ForwardVec = Relative.GetRotation().GetForwardVector();
	//ForwardVec *= 30.0f;	// 이정도가 최적화된 위치 (작으면 총이 너무 앞으로 오고, 크면 너무 멂)

	//NewSightVector += ForwardVec;
	//SightTransform.SetLocation(NewSightVector);
	//SightTransform.SetRotation(Relative.Rotator().Quaternion());

}

// 조준시 손의 위치를 계산
void UADAnimInstance::SetRelativeHandTransform()
{
	if(Character->GetCurrentOptic())
	{

		// optic의 S_Aim 소켓의 위치
		FTransform OpticSocketTransform = Character->GetCurrentOptic()->GetSocketTransform(FName("S_Aim"));
		FTransform MeshTransform = Character->GetMesh1P()->GetSocketTransform(FName("hand_r"));

		RelativeHandTransform = UKismetMathLibrary::MakeRelativeTransform(OpticSocketTransform, MeshTransform);	
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get Current optic"));

	}

}

void UADAnimInstance::SetFinalHandTransform()
{
	if (Character->GetCurrentOptic())
	{

		// optic의 S_Aim 소켓의 위치
		FTransform OpticSocketTransform = Character->GetCurrentOptic()->GetSocketTransform(FName("S_Aim"));
		FTransform MeshTransform = Character->GetMesh1P()->GetSocketTransform(FName("hand_r"));

		FinalHandTransform = UKismetMathLibrary::MakeRelativeTransform(OpticSocketTransform, MeshTransform);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get Current optic"));

	}
}

void UADAnimInstance::SetLeftHandIK()
{

	FTransform GunSocketTransform = Character->GetFPGun()->GetSocketTransform(FName("S_LeftHand"));
	FTransform MeshSocketTransform = Character->GetMesh1P()->GetSocketTransform(FName("hand_r"));

	// mesh(팔)을 총에 있는 그립 소켓에 맞는 위치를 계산해줌.
	LeftHandTransform = UKismetMathLibrary::MakeRelativeTransform(GunSocketTransform, MeshSocketTransform);
}

void UADAnimInstance::InterpAiming(float DeltaSeconds)
{
	AimAlpha = UKismetMathLibrary::FInterpTo(AimAlpha, static_cast<float>(bIsAiming), DeltaSeconds, 10.0f);

	if(AimAlpha >= 1.0f || AimAlpha <= 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("Alpha : %f"), AimAlpha);

		bInterpAiming = false;
	}
}

void UADAnimInstance::InterpRelativeHand(float DeltaSeconds)
{
	RelativeHandTransform = UKismetMathLibrary::TInterpTo(RelativeHandTransform, FinalHandTransform, DeltaSeconds, 10.0f);
	// 기존 부착물 위치에서 다른 부착물 위치로 손을 옮길때 보간하는데 보간이 끝나면(equals) false로.
	if(RelativeHandTransform.Equals(FinalHandTransform))
	{
		bInterpRelativeHand = false;
	}
}

void UADAnimInstance::MoveVectorCurve(float DeltaSeconds)
{
	if(VectorCurve)
	{
		FVector VelocityVec = Character->GetMovementComponent()->Velocity;
		VelocityVec.Z = 0.0f;
		float Velocity = VelocityVec.Size();
		float MaxSpeed = Character->GetMovementComponent()->GetMaxSpeed();
		Velocity = UKismetMathLibrary::NormalizeToRange(Velocity, (MaxSpeed/0.3f)*-1.0f, MaxSpeed);
		// 캐릭터가 이동시 만든 벡터 커브에 따라 총의 수직 흔들림을 구현.
		// 시간과 z의 곡선에 따라 현재 총위치와 보간하여 계산함.
		FVector NewVec = VectorCurve->GetVectorValue(Character->GetGameTimeSinceCreation());
		SwayLocation = UKismetMathLibrary::VInterpTo(SwayLocation, NewVec, DeltaSeconds, 1.8f)*Velocity;

		SwayLocation *= Velocity;

		//SightTransform.SetLocation(SwayLocation);
	}
	
}

// 화면을 움직일때 무기의 상하좌우 흔들림 구현 (타르코프)
void UADAnimInstance::RotateWithRotation(float DeltaSeconds)
{
	FRotator CurrentRotation = Character->GetControlRotation();
	TurnRotation = UKismetMathLibrary::RInterpTo(TurnRotation, CurrentRotation - OldRotation, DeltaSeconds, 3.0f);
	TurnRotation.Roll = TurnRotation.Pitch * -1.0;	// 화면을 상하로 움직일시 무기의 흔들림

	// 급격한 움직임에서 너무 큰 움직임을 보이지 않도록 제한함.
	TurnRotation.Yaw = FMath::Clamp(TurnRotation.Yaw, -7.0f, 7.0f);
	TurnRotation.Roll = FMath::Clamp(TurnRotation.Roll, -3.0f, 3.0f);

	TurnLocation.X = TurnRotation.Yaw/4.0f;
	TurnLocation.Z = TurnRotation.Roll/1.5f;

	OldRotation = CurrentRotation;
}

void UADAnimInstance::InterpFinalRecoil(float DeltaSeconds)
{// interp to zero
	FinalRecoilTransform = UKismetMathLibrary::TInterpTo(FinalRecoilTransform, FTransform(), DeltaSeconds, 10.0f);

}

void UADAnimInstance::InterpRecoil(float DeltaSeconds)
{	//interp to finalrecoiltransform
	RecoilTransform =  UKismetMathLibrary::TInterpTo(RecoilTransform, FinalRecoilTransform, DeltaSeconds, 10.0f);

}

void UADAnimInstance::SetAiming(bool IsAiming)
{
	if(bIsAiming != IsAiming)
	{
		bIsAiming = IsAiming;
		bInterpAiming = true;
	}
}

void UADAnimInstance::CycledOptic()
{
	// 부착물 전환시 새로운(다른) 부착물의 조준위치를 다시 계산하는
	// 카메라 앞으로 오는 위치는 동일하므로 재계산x, 손위치만 재계산o
	//SetSightTransform();
	SetFinalHandTransform();
	bInterpRelativeHand = true;

}

void UADAnimInstance::Reload()
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

void UADAnimInstance::StopReload()
{
	ReloadAlpha = 1.0f;
}

void UADAnimInstance::Fire()
{
	FVector RecoilLoc;
	FRotator RecoilRot;
	if(bIsAiming)
	{
		RecoilLoc = FinalRecoilTransform.GetLocation();
		// 사격시 총이 뒤로 밀려나는 효과를 내는 벡터
		//RecoilLoc += FVector(0.0f, -5.0f, 0.0f);
		// x 좌우로 흔들림, y 뒤로 밀림
		RecoilLoc += FVector(FMath::RandRange(-0.3f, 0.3f), FMath::RandRange(-2.0f, -1.0f), FMath::RandRange(0.2f, 1.0f));

		RecoilRot = FinalRecoilTransform.GetRotation().Rotator();
		// p, y 좌우 반동, r = 수직반동 (-3은 안정적인 반동)
		RecoilRot += FRotator(FMath::RandRange(-5.0f,5.0f), FMath::RandRange(-1.0f, 1.0f), -5.0f);
		FinalRecoilTransform.SetRotation(RecoilRot.Quaternion());
		FinalRecoilTransform.SetLocation(RecoilLoc);	
	}
	else
	{
		RecoilLoc = FinalRecoilTransform.GetLocation();
		// 사격시 총이 뒤로 밀려나는 효과를 내는 벡터
		//RecoilLoc += FVector(0.0f, -5.0f, 0.0f);
		// x 좌우로 흔들림, y 뒤로 밀림
		RecoilLoc += FVector(FMath::RandRange(-0.5f, 0.5f), FMath::RandRange(-5.0f, -1.0f), FMath::RandRange(0.2f, 1.0f));
    
		RecoilRot = FinalRecoilTransform.GetRotation().Rotator();
		// p, y 좌우 반동, r = 수직반동 (-3은 안정적인 반동)
		RecoilRot += FRotator(FMath::RandRange(-10.0f,10.0f), FMath::RandRange(-1.0f, 1.0f), -15.0f);
		FinalRecoilTransform.SetRotation(RecoilRot.Quaternion());
		FinalRecoilTransform.SetLocation(RecoilLoc);	
	}
}
