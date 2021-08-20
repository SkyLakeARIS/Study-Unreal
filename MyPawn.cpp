// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ourCamera"));
	mCamera->SetupAttachment(RootComponent);
	mCamera->SetWorldLocation(FVector(-250.0f, 0.0f, 250.0f));
	mCamera->SetWorldRotation(FRotator(-45.0f, 0.0f, 0.0f));

	mVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ourVisibleComponent"));
	mVisibleComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*
	*	���� ��ǥ
		�Ϲ� TPS���� ���Ӱ� ���� ī�޶� ��ü �߽��� �ٶ󺸸� ������ �����̰�,
		��ü�� ī�޶� �ٶ󺸴� ������ �������� �̵��Ѵ�.
		ī�޶� �������� �ʰ� ��ü�� ���۽� ī�޶� ������ ������ ä ĳ���͸� �����̵��� �Ѵ�.
		(��ü�� �̵��Կ� ���� ī�޶� ���� ���� �̵��ϴ� �κ��� �����ϰ� �����.)
	*/
	{
		if (mTheta != 0)
		{
			FVector nowCameraLocation = mCamera->GetRelativeLocation();
			FVector newCameraLocation = nowCameraLocation;
			FRotator newCameraRotation = mCamera->GetRelativeRotation();
			float theta = mTheta * (PI/180);
			// nowCameraLocation ��ǥ���� theta��ŭ ȸ���� ��ǥ�� ���Ѵ�.
			newCameraLocation.X = nowCameraLocation.X * FMath::Cos(theta) - nowCameraLocation.Y * FMath::Sin(theta);
			newCameraLocation.Y = nowCameraLocation.X * FMath::Sin(theta) + nowCameraLocation.Y * FMath::Cos(theta);
			//ī�޶� ��ü �߽��� �������� ȸ���Ҷ� ��� ��ü�� ��� �ٶ�����ϹǷ�
			//ī�޶� ��ü ������ �����̴� ��ŭ ī�޶� ��(Yaw)�� mTheta ��ŭ ȸ����Ų��.
			newCameraRotation.Yaw += mTheta;
			mCamera->SetRelativeRotation(newCameraRotation);
			mCamera->SetRelativeLocation(newCameraLocation);
			/*
				�ﰢ�Լ����� mTheta�� �������� �ٲ��� ������ ȸ���� �ſ� ������ �Ǵµ�,
				�ﰢ�Լ����� �������� �ٲ���ϴ� ������ ��Ȯ�ϰ� 
				�������� �������Ƿ� �� �� ���ΰ� �ʿ��� �κ�.

			*/
		}
	}

	{
		if (!mCurrentVelocity.IsZero())
		{
			/*
			* Ʃ�丮�󿡼��� ������ ��ǥ�迡(?) ���ؼ� ��, ��, ������ �������µ�, ���� ���غô� ���ӿ�����
			* ��κ� ī�޶� �ٶ󺸴� ������ ��ü�� �̵��ϴ� ��ǥ���� ������ �Ǵ� �κ��� �Ϲ����̾����Ƿ� �̸� �����Ͽ�
			* ī�޶� �ٶ󺸴� �������� ��, ��, ���� �����̵��� �ϴ°��� �������� ��ȯ ���̴�.
			* 
			* �� �������� WŰ�� ������ ī�޶� �ٶ󺸴� ���⿡ ���� ���ư��� ��, �ڿ� ���ؼ��� ������� �ʰ�
			* ������ ���ư��⸸ �ϴ� ����.
			* 
			* ī�޶� �ٶ󺸴� ���⿡ ���� ��ü�� �� �������� �����̴� ���� ����������, ��ü�� ī�޶��� �������� ȸ���ϴ� ������ 
			* ���� �������� �ʾҴ�.
			* ���� ����� �õ� �غ����� ��ü�� ȸ���� �Բ� ī�޶� ���� ȸ���� �ż� ī�޶� ��ü�� �������̰� �� �ʿ䰡 �ִٰ� �����ȴ�.
			* �ٸ� �׷��� ���� �� ������ ��ü�� �̵��� �Բ� ī�޶� ������� ���ϹǷ� ������ �� �ʿ��� ������ �����Ѵ�.
			*/
			FVector forwardOfCamera = FVector(mCamera->GetForwardVector().X, mCamera->GetForwardVector().Y , 0.0f);
			forwardOfCamera.Normalize();
			FVector rightOfCamera = FVector(mCamera->GetRightVector().X, mCamera->GetRightVector().Y ,0.0f);
			rightOfCamera.Normalize();

			FVector forwardOfActor = FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0.0f);
			forwardOfActor.Normalize();

			float theta = FMath::Acos(FVector::DotProduct(forwardOfActor, forwardOfCamera));
			theta *= (180 / PI);

			float dir = mTheta;
			if (mTheta > 0.0f)
			{
				dir = 1;
			}
			else if(mTheta < 0)
			{
				dir = -1;
			}
			theta *= dir;
			if (dir != 0.0f)
			{
				/*
					ī�޶� �����̸鼭 ��ü�� ���۽� ��ü ȸ�� ���¿� ���� ī�޶� ȸ������ ���� ī�޶� ȸ������ �����ϰ�
					����Ǵ� ������ �����.

					����� ī�޶� ��ü�� ���ӵǾ �߻��ϴ� ������ �´ٰ� �Ǵ��Ͽ� 
					ī�޶� ���������� �����̰� �� �� �ֵ��� ����� ã�� �ʿ䰡 �ִ�.
					
				*/
				FRotator newActorRotation = GetActorRotation();
				newActorRotation.Yaw = theta;
				SetActorRotation(newActorRotation);
			}

			FVector newActorLocation = GetActorLocation() + ((mCurrentVelocity.X * forwardOfCamera + mCurrentVelocity.Y * rightOfCamera)* DeltaTime);
			SetActorLocation(newActorLocation);
		}

	
	}



}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyPawn::MoveRight);
	PlayerInputComponent->BindAxis("MoveMouseX", this, &AMyPawn::MoveMouseX);

}

void AMyPawn::MoveForward(float value)
{
	mCurrentVelocity.X = FMath::Clamp(value, -1.0f, 1.0f) * 100.0f;
}

void AMyPawn::MoveRight(float value)
{
	mCurrentVelocity.Y = FMath::Clamp(value, -1.0f, 1.0f) * 100.0f;
}

void AMyPawn::MoveMouseX(float value)
{
	mTheta = value;
}