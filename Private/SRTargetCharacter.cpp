// Fill out your copyright notice in the Description page of Project Settings.


#include "SRTargetCharacter.h"
#include "SRPlayerController.h"
#include "SRSpawnPoint.h"
#include "SRTargetManager.h"

// Sets default values
ASRTargetCharacter::ASRTargetCharacter()
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
void ASRTargetCharacter::BeginPlay()
{
	Super::BeginPlay();

	mStartLocation = GetActorLocation();
	mEndLocation = GetActorLocation();

	GetMesh()->PlayAnimation(mPlateDownAnimation, false);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mHead->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASRTargetCharacter::dead()
{
	Destroy();
}

// Called every frame
void ASRTargetCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(mEndLocation != mStartLocation &&  mbIsMovable)
	{
		if (mbIsReturn == false)
		{
			auto pos = mEndLocation - GetActorLocation();
			auto loss = pos;
			pos.Normalize();
			SetActorLocation(GetActorLocation() + pos * mSpeedFactor);
			if (loss.Size() < 10.0f)
			{
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
				mbIsReturn = !mbIsReturn;
			}
		}

	}

}

void ASRTargetCharacter::SetTargetType(bool isCharacterType)
{
	mbIsCharacterType = isCharacterType;
	if(mbIsCharacterType)
	{
		GetMesh()->SetSkeletalMesh(mCharacterMesh);
		mHead->SetSphereRadius(13.5f);
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
		mHead->SetRelativeLocation(FVector(2.437271f, 0.000015f, 155.0f));
	}
	mHead->SetCollisionObjectType(ECC_Pawn);
	mHead->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ASRTargetCharacter::ActiveTarget()
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

void ASRTargetCharacter::BindTargetManager(ASRTargetManager* targetManager)
{
	mOnTargetDown.AddUObject(targetManager, &ASRTargetManager::RandomTargetSpawn);
}

void ASRTargetCharacter::BindSpawnPoint(ASRSpawnPoint* spawnPoint)
{
	mOnTargetDown.AddUObject(spawnPoint, &ASRSpawnPoint::DeActive);
}

/*
 *  타겟이 총알에 맞았을때 실행하는 함수입니다.
 *	전달된 데미지만큼 타겟 체력을 감소하고 점수를 포인터로 반환합니다.
 *	타겟이 다운되면 true, 그렇지 않으면 false를 반환합니다.
 *	타겟은 다운시 타겟매니저에게 알립니다.
 */
bool ASRTargetCharacter::OnHit(int32 damage, int32* scoreOut)
{

	if(mHP > 0)
	{
		mHP -= damage;
		*scoreOut = mHitScore;
		UE_LOG(LogTemp, Warning, TEXT("ASRTargetCharacter :OnHit HP[%d]"), mHP);

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
		GetWorld()->GetTimerManager().SetTimer(mDeathTimer, this, &ASRTargetCharacter::dead, 3.0f, false,-1);

		if(mbIsMovable)
		{
			mbIsMovable = false;
		}

		return true;
	}
	return false;
}

int32 ASRTargetCharacter::GetHP()
{
	return mHP;
}

/*
 * 타겟이 활성화 상태인지 아닌지를 반환합니다.
 */
bool ASRTargetCharacter::IsActive()
{
	return !mbIsDown;
}

void ASRTargetCharacter::SetEndLocation(float distance, EMovableAxis direction)
{
	mDistance = distance;

	if(direction == EMovableAxis::X)
	{
		mEndLocation.X += distance;
		UE_LOG(LogTemp, Warning, TEXT("ASRTargetCharacter - SetEndLocation : X , distance %f"), mDistance);
	}
	else
	{
		mEndLocation.Y += distance;
		UE_LOG(LogTemp, Warning, TEXT("ASRTargetCharacter - SetEndLocation : Y , distance %f"), mDistance);
	}
	mDirection = mEndLocation - GetActorLocation();
	mDirection.Normalize();
}

void ASRTargetCharacter::SetMovable(bool isMovable)
{
	mbIsMovable = isMovable;
}

void ASRTargetCharacter::SetSpeed(float newSpeed)
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

void ASRTargetCharacter::SetCrouchable(bool canCrouch)
{
	mbCanCrouch = canCrouch;
}


