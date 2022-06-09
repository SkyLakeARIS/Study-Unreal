// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Version.h"
#include "GameModeData.h"
#include "SRPlayerState.h"
#include "GameFramework/Actor.h"
#include "VersionProjectile.generated.h"

enum class EWeaponType : uint8;

class USphereComponent;
class UProjectileMovementComponent;

DECLARE_DELEGATE(FOnHitTarget)
DECLARE_DELEGATE(FOnUpdateKill)
DECLARE_DELEGATE_OneParam(FOnUpdateScore, int32)


enum class EHitType : uint8
{
	Hit,
	HeadShot,
	Kill
};

UCLASS(config=Game)
class AVersionProjectile : public AActor
{
	GENERATED_BODY()

public:

	AVersionProjectile();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void BindPlayerStateInfo(ASRPlayerState* playerState);

	UFUNCTION()
	void SetBulletType(EWeaponType gunType);

	void SetStartLocation(FVector location);


	void SetDebugMode(bool active);

protected:

	virtual void BeginPlay() override;

	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
	FOnHitTarget onHitAndUpdateAcc;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UParticleSystem* ImpactParticles;

private:

	bool mbDebugMode;
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	FVector mStartLocation;
	FOnUpdateScore mOnUpdateScore;
	FOnUpdateKill mOnUpdateKill;
	FWeaponDamage mDamageTable;
	EWeaponType mBulletType;
	EHitType mHitType;
	int32 mBulletDamage;
	bool mbIsCollision;
	bool mbIsHeadshot;
	bool mbIsTargetHit;
};

