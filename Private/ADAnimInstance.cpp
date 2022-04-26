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

		// �ʹ� ������ �ʱ�ȭ���ϸ� mesh����(������) nullptr�� �� ���ɼ��� �����Ƿ� �����̸� ��.
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

	// �߻��� ���� ����
	if(!RecoilTransform.Equals(FTransform()) || !FinalRecoilTransform.Equals(FTransform()))
	{
		InterpRecoil(DeltaSeconds);
		InterpFinalRecoil(DeltaSeconds);
	}


	SetLeftHandIK();
}

// ���ؽ� �������� ��ġ�� ��ġ�� ���
void UADAnimInstance::SetSightTransform()
{
	FTransform CamTransform = Character->GetFirstPersonCameraComponent()->GetComponentTransform();
	FTransform MeshTransform = Character->GetMesh1P()->GetComponentTransform();

	SightTransform = UKismetMathLibrary::MakeRelativeTransform(CamTransform, MeshTransform);

	SightTransform.SetLocation(SightTransform.GetLocation() + SightTransform.GetRotation().Vector() * 30.0f);
	//// ���� �������� mesh(��+��+������)��ġ�� ���Ϸ��� ������.
	//FVector NewSightVector = Relative.GetLocation();

	//FVector ForwardVec = Relative.GetRotation().GetForwardVector();
	//ForwardVec *= 30.0f;	// �������� ����ȭ�� ��ġ (������ ���� �ʹ� ������ ����, ũ�� �ʹ� ��)

	//NewSightVector += ForwardVec;
	//SightTransform.SetLocation(NewSightVector);
	//SightTransform.SetRotation(Relative.Rotator().Quaternion());

}

// ���ؽ� ���� ��ġ�� ���
void UADAnimInstance::SetRelativeHandTransform()
{
	if(Character->GetCurrentOptic())
	{

		// optic�� S_Aim ������ ��ġ
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

		// optic�� S_Aim ������ ��ġ
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

	// mesh(��)�� �ѿ� �ִ� �׸� ���Ͽ� �´� ��ġ�� �������.
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
	// ���� ������ ��ġ���� �ٸ� ������ ��ġ�� ���� �ű涧 �����ϴµ� ������ ������(equals) false��.
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
		// ĳ���Ͱ� �̵��� ���� ���� Ŀ�꿡 ���� ���� ���� ��鸲�� ����.
		// �ð��� z�� ��� ���� ���� ����ġ�� �����Ͽ� �����.
		FVector NewVec = VectorCurve->GetVectorValue(Character->GetGameTimeSinceCreation());
		SwayLocation = UKismetMathLibrary::VInterpTo(SwayLocation, NewVec, DeltaSeconds, 1.8f)*Velocity;

		SwayLocation *= Velocity;

		//SightTransform.SetLocation(SwayLocation);
	}
	
}

// ȭ���� �����϶� ������ �����¿� ��鸲 ���� (Ÿ������)
void UADAnimInstance::RotateWithRotation(float DeltaSeconds)
{
	FRotator CurrentRotation = Character->GetControlRotation();
	TurnRotation = UKismetMathLibrary::RInterpTo(TurnRotation, CurrentRotation - OldRotation, DeltaSeconds, 3.0f);
	TurnRotation.Roll = TurnRotation.Pitch * -1.0;	// ȭ���� ���Ϸ� �����Ͻ� ������ ��鸲

	// �ް��� �����ӿ��� �ʹ� ū �������� ������ �ʵ��� ������.
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
	// ������ ��ȯ�� ���ο�(�ٸ�) �������� ������ġ�� �ٽ� ����ϴ�
	// ī�޶� ������ ���� ��ġ�� �����ϹǷ� ����x, ����ġ�� ����o
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
		// ��ݽ� ���� �ڷ� �з����� ȿ���� ���� ����
		//RecoilLoc += FVector(0.0f, -5.0f, 0.0f);
		// x �¿�� ��鸲, y �ڷ� �и�
		RecoilLoc += FVector(FMath::RandRange(-0.3f, 0.3f), FMath::RandRange(-2.0f, -1.0f), FMath::RandRange(0.2f, 1.0f));

		RecoilRot = FinalRecoilTransform.GetRotation().Rotator();
		// p, y �¿� �ݵ�, r = �����ݵ� (-3�� �������� �ݵ�)
		RecoilRot += FRotator(FMath::RandRange(-5.0f,5.0f), FMath::RandRange(-1.0f, 1.0f), -5.0f);
		FinalRecoilTransform.SetRotation(RecoilRot.Quaternion());
		FinalRecoilTransform.SetLocation(RecoilLoc);	
	}
	else
	{
		RecoilLoc = FinalRecoilTransform.GetLocation();
		// ��ݽ� ���� �ڷ� �з����� ȿ���� ���� ����
		//RecoilLoc += FVector(0.0f, -5.0f, 0.0f);
		// x �¿�� ��鸲, y �ڷ� �и�
		RecoilLoc += FVector(FMath::RandRange(-0.5f, 0.5f), FMath::RandRange(-5.0f, -1.0f), FMath::RandRange(0.2f, 1.0f));
    
		RecoilRot = FinalRecoilTransform.GetRotation().Rotator();
		// p, y �¿� �ݵ�, r = �����ݵ� (-3�� �������� �ݵ�)
		RecoilRot += FRotator(FMath::RandRange(-10.0f,10.0f), FMath::RandRange(-1.0f, 1.0f), -15.0f);
		FinalRecoilTransform.SetRotation(RecoilRot.Quaternion());
		FinalRecoilTransform.SetLocation(RecoilLoc);	
	}
}
