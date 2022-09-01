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

	// common initialize
	mHP = MAX_HP;

	mbIsCharacterType = true;
	mEndLocation = FVector::ZeroVector;
	mbIsCrouching = false;
	mbToEndLocation = true;

	// character type assets load
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

	// plate type assets load
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
	mEndLocation = FVector::ZeroVector;

	GetMesh()->PlayAnimation(mPlateDownAnimation, false);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	changeCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASRTargetCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	mOnTargetDown.Clear();
}

void ASRTargetCharacter::changeCollisionEnabled(ECollisionEnabled::Type newType)
{
	mHead->SetCollisionEnabled(newType);
	GetMesh()->SetCollisionEnabled(newType);
}

// Called every frame
void ASRTargetCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (mbIsMovable == false)
	{
		return;
	}

	if((mEndLocation - GetActorLocation()).Size() < 10.0f && mbToEndLocation)
	{
		mDistanceToMoveEveryTime = -mDistanceToMoveEveryTime;
		mbToEndLocation = false;
	}
	else if((mStartLocation - GetActorLocation()).Size() < 10.0f && mbToEndLocation == false)
	{
		mDistanceToMoveEveryTime = -mDistanceToMoveEveryTime;
		mbToEndLocation = true;
	}
	SetActorLocation(GetActorLocation() + mDistanceToMoveEveryTime);
}

void ASRTargetCharacter::SetTarget(bool isCharacterType, bool isMovable, bool isCrouchable)
{
	mbIsCharacterType = isCharacterType;
	mbIsMovable = isMovable;
	mbIsCrouchable = isCrouchable;

	if(mbIsCharacterType)
	{
		GetMesh()->SetSkeletalMesh(mCharacterMesh);
		mHead->SetSphereRadius(13.5f);
		mHead->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("S_Head"));
	}
	else
	{
		GetMesh()->SetSkeletalMesh(mPlateMesh);
		mHead->SetSphereRadius(18.0f);
		mHead->SetRelativeLocation(FVector(2.437271f, 0.000015f, 155.0f));
	}

	mHead->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
}

// 타겟을 실제로 맞추고 처리할 수 있는 상태가 됩니다.
void ASRTargetCharacter::ActiveTarget()
{
	mHP = MAX_HP;

	changeCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	if(mbIsCharacterType)
	{
		// 캐릭터 포즈가 앉을 수 있는(스폰포인트이면) 상태이면 랜덤으로 포즈를 정함.
		if (mbIsCrouchable && FMath::RandBool())
		{
			GetMesh()->PlayAnimation(mCharacterCrouchPoseAnimation, true);
			mbIsCrouching = true;
		}
		else
		{
			GetMesh()->PlayAnimation(mCharacterStandPoseAnimation, true);
		}
	}
	else
	{
		GetMesh()->PlayAnimation(mPlateUpAnimation, false);
	}

	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), mPlateUpSound, GetActorLocation());
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
bool ASRTargetCharacter::OnHit(int32 damage, int32* outScore)
{
	checkf(outScore != nullptr, TEXT("ASRTargetCharacter-OnHit scoreOut이 null입니다."));
	checkf(damage > 0, TEXT("ASRTargetCharacter-OnHit damage가 0이하 입니다."));

	if(mHP > MIN_HP)
	{
		mHP -= damage;
		*outScore = HIT_SCORE;
	}

	if (!mbIsCharacterType)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), mPlateHitSound, GetActorLocation());
	}

	if(mHP <= MIN_HP)
	{
		*outScore = KILL_SCORE;

		if(mbIsCharacterType)
		{
			if(mbIsCrouching)
			{
				GetMesh()->PlayAnimation(mCharacterCrouchDeadAnimation, false);
			}
			else
			{
				GetMesh()->PlayAnimation(mCharacterStandDeadAnimation, false);
			}
		}
		else
		{
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), mPlateDownSound, GetActorLocation());
			GetMesh()->PlayAnimation(mPlateDownAnimation, false);
		}

		changeCollisionEnabled(ECollisionEnabled::NoCollision);

		mOnTargetDown.Broadcast();
		
		SetLifeSpan(3.0f);
		if(mbIsMovable)
		{
			mbIsMovable = false;
		}

		return true;
	}
	return false;
}

void ASRTargetCharacter::initializeMovement(FVector endLocation, float speedFactor)
{
	checkf(speedFactor >= 1.0f, TEXT("ASRTargetCharacter - SetEndLocation : speedFactor는 1.0f이상이어야 합니다."));

	mEndLocation = endLocation;
	mDistanceToMoveEveryTime = mEndLocation - mStartLocation;
	mDistanceToMoveEveryTime.Normalize();
	mDistanceToMoveEveryTime *= speedFactor;
}


