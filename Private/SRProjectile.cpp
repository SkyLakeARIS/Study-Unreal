// Copyright Epic Games, Inc. All Rights Reserved.
#include "SRProjectile.h"
#include "SRPlayerController.h"
#include "DrawDebugHelpers.h"
#include "SRTargetCharacter.h"
#include "SRPlayerCharacter.h"
#include "UIHUDWidget.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

ASRProjectile::ASRProjectile() 
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(1.0f);
	CollisionComp->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ASRProjectile::OnHit);		// set up a notification for when this component hits something blocking

	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));


	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_TRACER(TEXT("/Game/MilitaryWeapDark/FX/P_Pistol_Tracer_01.P_Pistol_Tracer_01"));
	if(PS_TRACER.Succeeded())
	{
		mBulletTrace = PS_TRACER.Object;
	}

	InitialLifeSpan = 10.0f;
	mbIsCollision = false;
	mbIsTargetHit = false;
	mbDebugMode = false;
}

void ASRProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (mbIsCollision == false && mbDebugMode)
	{
		DrawDebugPoint(GetWorld(), GetActorLocation(), 5.0f, FColor::Green, false, 15.0f);
	}
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), mBulletTrace, FTransform(GetActorRotation(), GetActorLocation()));
}

void ASRProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ASRProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	mbIsCollision = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint));

	// Ÿ�ٿ� ������ ��� �ܿ��� ������ �浹 ó���� ���� �ʽ��ϴ�.
	if (!Hit.GetActor()->GetName().Contains(FString("TargetCharacter")))
	{
		Destroy();
		return;
	}

	mHitType = EHitType::Hit;
	const float distanceToHitPoint = (GetActorLocation() - mStartLocation).Size();

	// �Ѿ� Ÿ�Կ� �´� ������ ���̺��� �����Ͽ� �Ÿ��� ���� ���� �������� ����մϴ�.
	// 1�� cm��� �����Ͽ� 50���͸��� �������� 1�� �����մϴ�. ������ ���Ҵ� �ּڰ� �̸��� ���� �ʽ��ϴ�.
	switch(mBulletType)
	{
		case EWeaponType::AR:
			mBulletDamage = FMath::Clamp(mDamageTable.AR - static_cast<int32>(distanceToHitPoint / 5000.0f), mDamageTable.AR_Min, mDamageTable.AR);
			break;
		case EWeaponType::SR:
			mBulletDamage = FMath::Clamp(mDamageTable.SR - static_cast<int32>(distanceToHitPoint / 5000.0f), mDamageTable.SR_Min, mDamageTable.SR);
			break;
		case EWeaponType::HG:
			mBulletDamage = FMath::Clamp(mDamageTable.HG - static_cast<int32>(distanceToHitPoint / 5000.0f), mDamageTable.HG_Min, mDamageTable.HG);
			break;
		default:
			checkf(false, TEXT("ASRProjectile - OnHit - EWeaponType �ùٸ��� ���� enum Ÿ���Դϴ�."));
			break;
	}

	if (Hit.GetComponent()->GetName().Contains(FString("Head")))
	{
		mBulletDamage *= 1.5f;
		mbIsHeadshot = true;
		mHitType = EHitType::HeadShot;
	}

	int32 getScore = 0;
	const auto targetCharacter = Cast<ASRTargetCharacter>(Hit.GetActor());
	const bool bIsKill = targetCharacter->OnHit(mBulletDamage, &getScore);

	getScore = mbIsHeadshot ? getScore * 1.5f : getScore;
	onHitAndUpdateAcc.Execute();

	if(bIsKill)
	{
		// �������� ��쿡 Ÿ���� ����Ʈ���� �� �Ÿ��� ���� �߰� ������ ����ϴ�.
		if(mBulletType == EWeaponType::SR)
		{
			getScore += static_cast<int32>(distanceToHitPoint);
		}
		mHitType = EHitType::Kill;
		mOnUpdateKill.Execute();
	}
	mOnUpdateScore.Execute(getScore);
	mHitmark.Execute(mHitType);
	
	Destroy();
}

/*
 * playerstate�� ������ ���ε��մϴ�.(delegate)
 * ��� ����: Hit count, accuracy, score, kill count
 */
void ASRProjectile::BindPlayerStateInfo(ASRPlayerState* srPlayerState)
{
	onHitAndUpdateAcc.BindUObject(srPlayerState, &ASRPlayerState::OnHitCount);
	mOnUpdateScore.BindUObject(srPlayerState, &ASRPlayerState::OnAddScore);
	mOnUpdateKill.BindUObject(srPlayerState, &ASRPlayerState::OnKill);
}

/*
 * player controller�� ������ ���ε��մϴ�.(delegate)
 * ��� ����: HUDWidget
 */
void ASRProjectile::BindHUDWidget(UUIHUDWidget* hud)
{
	mHitmark.BindUObject(hud, &UUIHUDWidget::AddHitMarkToViewPort);
}

/*
 * �߻�� �Ѿ��� Ÿ���� ����Ÿ�����κ��� ���ϴ� �Լ��Դϴ�.
 */
void ASRProjectile::SetBulletType(EWeaponType gunType)
{
	mBulletType = gunType;
}

void ASRProjectile::SetStartLocation(FVector location)
{
	mStartLocation = location;
}

void ASRProjectile::SetDebugMode(bool active)
{
	mbDebugMode = active;
}
