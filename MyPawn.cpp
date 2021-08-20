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
	*	구현 목표
		일반 TPS시점 게임과 같이 카메라가 물체 중심을 바라보며 주위를 움직이고,
		물체가 카메라가 바라보는 방향을 기준으로 이동한다.
		카메라를 움직이지 않고 물체만 조작시 카메라 시점은 고정된 채 캐릭터만 움직이도록 한다.
		(물체가 이동함에 따라 카메라도 같이 따라 이동하는 부분은 동일하게 적용됨.)
	*/
	{
		if (mTheta != 0)
		{
			FVector nowCameraLocation = mCamera->GetRelativeLocation();
			FVector newCameraLocation = nowCameraLocation;
			FRotator newCameraRotation = mCamera->GetRelativeRotation();
			float theta = mTheta * (PI/180);
			// nowCameraLocation 좌표에서 theta만큼 회전한 좌표를 구한다.
			newCameraLocation.X = nowCameraLocation.X * FMath::Cos(theta) - nowCameraLocation.Y * FMath::Sin(theta);
			newCameraLocation.Y = nowCameraLocation.X * FMath::Sin(theta) + nowCameraLocation.Y * FMath::Cos(theta);
			//카메라가 물체 중심을 기준으로 회전할때 렌즈도 물체를 계속 바라봐야하므로
			//카메라가 물체 주위를 움직이는 만큼 카메라 축(Yaw)도 mTheta 만큼 회전시킨다.
			newCameraRotation.Yaw += mTheta;
			mCamera->SetRelativeRotation(newCameraRotation);
			mCamera->SetRelativeLocation(newCameraLocation);
			/*
				삼각함수에서 mTheta를 라디안으로 바꾸지 않으면 회전에 매우 빠르게 되는데,
				삼각함수에만 라디안으로 바꿔야하는 이유를 명확하게 
				인지하지 못했으므로 좀 더 공부가 필요한 부분.

			*/
		}
	}

	{
		if (!mCurrentVelocity.IsZero())
		{
			/*
			* 튜토리얼에서는 고정된 좌표계에(?) 대해서 앞, 뒤, 옆으로 움직였는데, 보통 접해봤던 게임에서는
			* 대부분 카메라가 바라보는 방향이 물체가 이동하는 좌표계의 기준이 되는 부분이 일반적이었으므로 이를 수정하여
			* 카메라가 바라보는 방향으로 앞, 뒤, 옆을 움직이도록 하는것을 목적으로 변환 중이다.
			* 
			* 현 문제점은 W키를 누르면 카메라가 바라보는 방향에 따라 나아가나 옆, 뒤에 대해서도 적용되지 않고
			* 앞으로 나아가기만 하는 문제.
			* 
			* 카메라가 바라보는 방향에 따라서 물체가 그 방향으로 움직이는 것을 구현했으나, 물체가 카메라의 방향으로 회전하는 문제가 
			* 아직 고쳐지지 않았다.
			* 여러 방법을 시도 해봤으나 물체의 회전과 함께 카메라도 같이 회전이 돼서 카메라가 물체에 독립적이게 할 필요가 있다고 생각된다.
			* 다만 그렇게 했을 때 문제는 물체의 이동과 함께 카메라가 따라오지 못하므로 연산이 또 필요할 것으로 예상한다.
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
					카메라를 움직이면서 물체를 조작시 물체 회전 상태에 따라 카메라 회전값도 같이 카메라 회전값과 동일하게
					변경되는 문제가 생긴다.

					상술한 카메라가 물체에 종속되어서 발생하는 문제가 맞다고 판단하여 
					카메라가 독립적으로 움직이게 할 수 있도록 방법을 찾을 필요가 있다.
					
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