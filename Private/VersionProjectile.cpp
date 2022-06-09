// Copyright Epic Games, Inc. All Rights Reserved.

#include "VersionProjectile.h"

#include "CharacterPlayerController.h"
#include "DrawDebugHelpers.h"
#include "TargetCharacter.h"
#include "VersionCharacter.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

AVersionProjectile::AVersionProjectile() 
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(1.0f);
	CollisionComp->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AVersionProjectile::OnHit);		// set up a notification for when this component hits something blocking


	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 75000.f;
	ProjectileMovement->MaxSpeed = 360000.f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
	// Die after 3 seconds by default
	InitialLifeSpan = 10.0f;
	mbIsCollision = false;
	mbIsTargetHit = false;
	mbDebugMode = false;
}

void AVersionProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (mbIsCollision == false && mbDebugMode)
	{
		DrawDebugPoint(GetWorld(), GetActorLocation(), 5.0f, FColor::Green, false, 15.0f);
	}
}

void AVersionProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AVersionProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	mbIsCollision = true;

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint));
	}

	if (Hit.GetActor()->GetName().Contains(FString("TargetCharacter")))
	{
		mbIsTargetHit = true;
		mHitType = EHitType::Hit;
	}

	AVersionCharacter* character = nullptr;
	
	if(mbIsTargetHit)
	{
		character = Cast<AVersionCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));


		// 1�� cm��� �����Ͽ� 50���͸��� �������� 1�� �����մϴ�. ������ ���Ҵ� �ּڰ� �̸��� ���� �ʽ��ϴ�.
		const float distanceToHitPoint = (GetActorLocation() - mStartLocation).Size();
		mBulletDamage -= (distanceToHitPoint / 500.0f);
		switch(mBulletType)
		{
			case EWeaponType::AR:
				mBulletDamage =  FMath::Clamp(mBulletDamage, mDamageTable.AR_Min, mDamageTable.AR);
				break;
			case EWeaponType::SR:
				mBulletDamage = FMath::Clamp(mBulletDamage, mDamageTable.SR_Min, mDamageTable.SR);
				break;
			case EWeaponType::HG:
				mBulletDamage = FMath::Clamp(mBulletDamage, mDamageTable.HG_Min, mDamageTable.HG);
				break;
			default:
				UE_LOG(LogTemp, Warning, TEXT("AVersionProjectile - OnHit - EWeaponType �ùٸ��� ���� enum Ÿ���Դϴ�."));
				break;
		}
		UE_LOG(LogTemp, Warning, TEXT("OnHit - distance: %f, bullet damage : %d"), distanceToHitPoint, mBulletDamage);

		if (Hit.GetComponent()->GetName().Contains(FString("Head")))
		{
			UE_LOG(LogTemp, Warning, TEXT("headshot!"));
			mBulletDamage *= 1.5f;
			mbIsHeadshot = true;
			mHitType = EHitType::HeadShot;

		}
		int32 getScore = 0;
		const auto targetCharacter = Cast<ATargetCharacter>(Hit.GetActor());
		const bool bIsKill = targetCharacter->OnHit(mBulletDamage, &getScore);

		getScore = mbIsHeadshot ? getScore * 1.5f : getScore;

		onHitAndUpdateAcc.Execute();
		mOnUpdateScore.Execute(getScore);
		if(bIsKill)
		{
			mHitType = EHitType::Kill;
			mOnUpdateKill.Execute();
		}
		character->AddViewPortHitMark(mHitType);
	}
	Destroy();
}

/*
 * playerstate�� ������ ���ε��մϴ�.(delegate)
 * ��� ����: Hit count, accuracy, score, kill count
 */
void AVersionProjectile::BindPlayerStateInfo(ASRPlayerState* srPlayerState)
{
	onHitAndUpdateAcc.BindUObject(srPlayerState, &ASRPlayerState::OnHitCount);
	mOnUpdateScore.BindUObject(srPlayerState, &ASRPlayerState::OnAddScore);
	mOnUpdateKill.BindUObject(srPlayerState, &ASRPlayerState::OnKill);
}

/*
 * �߻�� �Ѿ��� Ÿ���� ����Ÿ�����κ��� ���ϴ� �Լ��Դϴ�.
 * �Ѿ��� Ÿ���� ���ϰ� �׿��´� �Ѿ��� �ӵ��� �������� ���մϴ�.
 */
void AVersionProjectile::SetBulletType(EWeaponType gunType)
{
	mBulletType = gunType;
	switch(mBulletType)
	{
		case EWeaponType::AR:
			mBulletDamage = mDamageTable.AR;
			break;
		case EWeaponType::HG:
			mBulletDamage = mDamageTable.HG;
			break;
		case EWeaponType::SR:
			mBulletDamage = mDamageTable.SR;
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("SetBulletType : gunType - �߸��� enum ������ �Դϴ�."));
			_ASSERT(false);
	}
	ProjectileMovement->InitialSpeed = 750.f;
	ProjectileMovement->MaxSpeed = 10000.f;
}

void AVersionProjectile::SetStartLocation(FVector location)
{
	mStartLocation = location;
}

void AVersionProjectile::SetDebugMode(bool active)
{
	mbDebugMode = active;
}
