// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "SRPlayerState.h"
#include "Version.h"
#include "GameFramework/Actor.h"
#include "VersionProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

DECLARE_DELEGATE(FOnHitTarget)

UCLASS(config=Game)
class AVersionProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

public:
	AVersionProjectile();

	virtual void Tick(float DeltaTime) override;

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void BindHitCheck(ASRPlayerState* playerState);
protected:
	virtual void BeginPlay() override;

	
protected:	
	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	FOnHitTarget* GetHitDelegate();

	FOnHitTarget onHitAndUpdateAcc;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UParticleSystem* ImpactParticles;

private:
	bool bIsCollision;
};
