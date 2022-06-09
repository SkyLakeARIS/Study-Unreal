// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetCharacter.h"
#include "CharacterPlayerController.h"
#include "SRPlayerState.h"
#include "SRSpawnPoint.h"
#include "SRTargetManager.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ATargetCharacter::ATargetCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you dont need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(42.f, 88.0f);


	// Dont rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	mHead = CreateDefaultSubobject<USphereComponent>(TEXT("Head"));
	mHead->SetupAttachment(GetMesh());

	// common init
	mHP =100;
	mHitScore = 100;
	mKillScore = 1000;
	mbIsCharacterType = true;
	mEndLocation = FVector::ZeroVector;
	mbIsReturn = false;
	mSpeedFactor = 2.0f;
	mDistance = 0.0f;
	// character type init
	mbIsStandPose = false;
	// plate type init
	mbIsDown = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CharacterMesh(TEXT("/Game/Assets/Target/HumanType/Meshes/MW_Style2_Male.MW_Style2_Male"));
	if(SK_CharacterMesh.Succeeded())
	{
		mCharacterMesh = SK_CharacterMesh.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimSequence> AS_CharacterStandPose(TEXT("/Game/Assets/Target/HumanType/Animations/Idle_Rifle_Ironsights.Idle_Rifle_Ironsights"));
	if (AS_CharacterStandPose.Succeeded())
	{
		mCharacterStandPoseAnimation = AS_CharacterStandPose.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimSequence> AS_CharacterCrouchPose(TEXT("/Game/Assets/Target/HumanType/Animations/Crouch_Idle_Rifle_Ironsights.Crouch_Idle_Rifle_Ironsights"));
	if (AS_CharacterCrouchPose.Succeeded())
	{
		mCharacterCrouchPoseAnimation = AS_CharacterCrouchPose.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimSequence> AS_CharacterStandDeadPose(TEXT("/Game/Assets/Target/HumanType/Animations/Death_Ironsights_2.Death_Ironsights_2"));
	if (AS_CharacterStandDeadPose.Succeeded())
	{
		mCharacterStandDeadAnimation = AS_CharacterStandDeadPose.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimSequence> AS_CharacterCrouchDeadPose(TEXT("/Game/Assets/Target/HumanType/Animations/Crouch_Death__1__Anim_mixamo_com.Crouch_Death__1__Anim_mixamo_com"));
	if (AS_CharacterCrouchDeadPose.Succeeded())
	{
		mCharacterCrouchDeadAnimation = AS_CharacterCrouchDeadPose.Object;
	}


	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_PlateMesh(TEXT("/Game/Assets/Target/PlateType/Meshes/SK_Target.SK_Target"));
	if (SK_PlateMesh.Succeeded())
	{
		mPlateMesh = SK_PlateMesh.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> SB_PlateUpSound(TEXT("/Game/Assets/Target/PlateType/Sounds/S_TargetGoUp.S_TargetGoUp"));
	if (SB_PlateUpSound.Succeeded())
	{
		mPlateUpSound = SB_PlateUpSound.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> SB_PlateHitSound(TEXT("/Game/Assets/Target/PlateType/Sounds/S_TargetPlateHit.S_TargetPlateHit"));
	if (SB_PlateHitSound.Succeeded())
	{
		mPlateHitSound = SB_PlateHitSound.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimSequence> AS_PlateUpAnimation(TEXT("/Game/Assets/Target/PlateType/Animations/A_Target_Up.A_Target_Up"));
	if (AS_PlateUpAnimation.Succeeded())
	{
		mPlateUpAnimation = AS_PlateUpAnimation.Object;
	}
	static ConstructorHelpers::FObjectFinder<UAnimSequence> AS_PlateDownAnimation(TEXT("/Game/Assets/Target/PlateType/Animations/A_Target_Down.A_Target_Down"));
	if (AS_PlateDownAnimation.Succeeded())
	{
		mPlateDownAnimation = AS_PlateDownAnimation.Object;
	}
	

}

// Called when the game starts or when spawned
void ATargetCharacter::BeginPlay()
{
	Super::BeginPlay();

	mStartLocation = GetActorLocation();
	mEndLocation = GetActorLocation();

	GetMesh()->PlayAnimation(mPlateDownAnimation, false);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mHead->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//GetMesh()->SetSkeletalMesh();
}

void ATargetCharacter::dead()
{
	Destroy();
}

// Called every frame
void ATargetCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(mEndLocation != mStartLocation &&  mbIsMovable)
	{
		if (mbIsReturn == false)
		{
			auto pos = mEndLocation - GetActorLocation();
			auto loss = pos;
			UE_LOG(LogTemp, Warning, TEXT("ATargetCharacter - Tick out : %f"), loss.Size());
			pos.Normalize();
			SetActorLocation(GetActorLocation() + pos * mSpeedFactor);
			if (loss.Size() < 10.0f)
			{
		//		UE_LOG(LogTemp, Warning, TEXT("ATargetCharacter - Tick in : %f"), loss.Size());

				mbIsReturn = !mbIsReturn;
			}
		}
		else
		{
			auto pos = GetActorLocation() - mStartLocation;
			auto loss = pos;
			pos.Normalize();
			SetActorLocation(GetActorLocation() - pos * mSpeedFactor);
			if (loss.Size() < 10.0f)
			{
				//		UE_LOG(LogTemp, Warning, TEXT("ATargetCharacter - Tick in : %f"), loss.Size());

				mbIsReturn = !mbIsReturn;
			}
		}


	}

	//static float sumDistance = 0.0f;
	//if (mbIsMovable)
	//{
	//	if(mbIsReturn == false)
	//	{

	//		if (sumDistance >= mDistance)
	//		{
	//			sumDistance = 0.0f;
	//			mbIsReturn = true;
	//		}

	//		SetActorLocation(GetActorLocation() + mDirection * mSpeedFactor);
	//		//FVector equels(0.1f, 0.1f, 0.1f);
	//		sumDistance += mDirection.Size() * mSpeedFactor;
	//	}
	//	else
	//	{
	//		if (sumDistance >= mDistance)
	//		{
	//			//UE_LOG(LogTemp, Warning, TEXT("arrived to startpoint"));
	//			sumDistance = 0.0f;
	//			mbIsReturn = false;
	//		}

	//		SetActorLocation(GetActorLocation() + mDirection * mSpeedFactor);
	//		sumDistance += mDirection.Size() * mSpeedFactor;

	//	}
	//}

	//if(GetActorLocation().X > 1500.0f)
	//{
	//	SetActorLocation(mStartLocation);
	//}
}

//// Called to bind functionality to input
//void ATargetCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}

void ATargetCharacter::SetTargetType(bool isCharacterType)
{
	mbIsCharacterType = isCharacterType;
	if(mbIsCharacterType)
	{
		GetMesh()->SetSkeletalMesh(mCharacterMesh);
		mHead->SetSphereRadius(13.5f);
		//mHead->SetRelativeLocation(FVector(2.041100f, 6.557510f, 163.0f));
		mHead->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("S_Head"));
		// 캐릭터 포즈가 앉을 수 있는 스폰포인트이면 랜덤으로 포즈를 정함.
		if(mbCanCrouch)
		{
			mbIsStandPose = FMath::RandBool();
		}
		else
		{
			mbIsStandPose = true;
		}
	}
	else
	{
		GetMesh()->SetSkeletalMesh(mPlateMesh);
		mHead->SetSphereRadius(18.0f);
	//	mHead->SetRelativeLocation(FVector(2.437271, 0.000015, 71.346024));
		mHead->SetRelativeLocation(FVector(2.437271f, 0.000015f, 155.0f));
	}
	mHead->SetCollisionObjectType(ECC_Pawn);
	mHead->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ATargetCharacter::ActiveTarget()
{
	mbIsDown = false;
	mHP = 100;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	mHead->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	if(mbIsCharacterType)
	{
		if (mbIsStandPose)
		{
			GetMesh()->PlayAnimation(mCharacterStandPoseAnimation, true);

		}
		else
		{
			GetMesh()->PlayAnimation(mCharacterCrouchPoseAnimation, true);
		}
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), mPlateUpSound, GetActorLocation());

	}
	else
	{
		GetMesh()->PlayAnimation(mPlateUpAnimation, false);
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), mPlateUpSound, GetActorLocation());
	}

}

void ATargetCharacter::BindTargetManager(ASRTargetManager* targetManager)
{
	//mOnTargetDown.AddUObject(targetManager, &ASRTargetManager::RandomTargetActive);
	mOnTargetDown.AddUObject(targetManager, &ASRTargetManager::RandomTargetSpawn);

	
}

void ATargetCharacter::BindSpawnPoint(ASRSpawnPoint* spawnPoint)
{
	mOnTargetDown.AddUObject(spawnPoint, &ASRSpawnPoint::DeActive);
}

/*
 *  타겟이 총알에 맞았을때 실행하는 함수입니다.
 *	전달된 데미지만큼 타겟 체력을 감소하고 점수를 포인터로 반환합니다.
 *	타겟이 다운되면 true, 그렇지 않으면 false를 반환합니다.
 *	타겟은 다운시 타겟매니저에게 알립니다.
 */
bool ATargetCharacter::OnHit(int32 damage, int32* scoreOut)
{

	if(mHP > 0)
	{
		mHP -= damage;
		*scoreOut = mHitScore;
		UE_LOG(LogTemp, Warning, TEXT("ATargetCharacter :OnHit HP[%d]"), mHP);

		if (!mbIsCharacterType)
		{
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), mPlateHitSound, GetActorLocation());
		}
	}

	if(mHP <= 0)
	{
		/*
		 *  play dead anim and sound(if its plate
		 */
		*scoreOut = mKillScore;

		if(mbIsCharacterType)
		{
			if(mbIsStandPose)
			{
				GetMesh()->PlayAnimation(mCharacterStandDeadAnimation, false);
			}
			else
			{
				GetMesh()->PlayAnimation(mCharacterCrouchDeadAnimation, false);
			}
		}
		else
		{
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), mPlateDownSound, GetActorLocation());
			mbIsDown = true;
			GetMesh()->PlayAnimation(mPlateDownAnimation, false);
			mEndLocation = FVector::ZeroVector;
		}

		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		mHead->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		mOnTargetDown.Broadcast();
		GetWorld()->GetTimerManager().SetTimer(mDeathTimer, this, &ATargetCharacter::dead, 3.0f, false,-1);

		if(mbIsMovable)
		{
			mbIsMovable = false;
		}

		return true;
	}
	return false;
}

int32 ATargetCharacter::GetHP()
{
	return mHP;
}

/*
 * 타겟이 활성화 상태인지 아닌지를 반환합니다.
 */
bool ATargetCharacter::IsActive()
{
	return !mbIsDown;
}

void ATargetCharacter::SetEndLocation(float distance, EMovableAxis direction)
{
	mDistance = distance;

	if(direction == EMovableAxis::X)
	{
		mEndLocation.X += distance;
		UE_LOG(LogTemp, Warning, TEXT("ATargetCharacter - SetEndLocation : X , distance %f"), mDistance);
	}
	else
	{
		mEndLocation.Y += distance;
		UE_LOG(LogTemp, Warning, TEXT("ATargetCharacter - SetEndLocation : Y , distance %f"), mDistance);
	}
	mDirection = mEndLocation - GetActorLocation();
	mDirection.Normalize();
	//UE_LOG(LogTemp, Warning, TEXT("ATargetCharacter - SetEndLocation : nomalize = %f %f %f"), a.X, a.Y, a.Z );
}

void ATargetCharacter::SetMovable(bool isMovable)
{
	mbIsMovable = isMovable;
}

void ATargetCharacter::SetSpeed(float newSpeed)
{
	if(newSpeed >= 1.0f)
	{
		mSpeedFactor = newSpeed;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SetSpeed : newSpeed less than 1.0f, %f"), newSpeed);
	}
}

void ATargetCharacter::SetCrouchable(bool canCrouch)
{
	mbCanCrouch = canCrouch;
}


