// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Version.h"
#include "GameModeData.h"
#include "SRPlayerState.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SRProjectile.generated.h"

DECLARE_DELEGATE(FOnUpdateHitCount)
DECLARE_DELEGATE(FOnUpdateHeatshotCount)
DECLARE_DELEGATE(FOnUpdateKill)
DECLARE_DELEGATE_OneParam(FOnUpdateScore, int32)

UENUM(BlueprintType)
enum class EHitType : uint8
{
	Hit,
	HeadShot,
	Kill
};
DECLARE_DELEGATE_OneParam(FOnHitmark, EHitType)


class ASRPlayerController;
enum class eWeaponType : uint8;
class USphereComponent;
class UProjectileMovementComponent;


/*
 * �� �Ѿ˵��� �θ� Ŭ�����Դϴ�.
 * �浹ó��, ����ó���� �θ𿡼� ó���մϴ�.
 */

UCLASS(config=Game)
class ASRProjectile : public AActor
{
	GENERATED_BODY()

public:

	ASRProjectile();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void BindPlayerStateInfo(ASRPlayerState& playerState);

	void BindHUDWidget(UUIHUDWidget& hud);

	UFUNCTION()
	void SetBulletType(const eWeaponType gunType);
	void SetStartLocation(const FVector location);

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	static bool bIsDebugMode;

protected:
	FOnUpdateHitCount mOnHitAndUpdateAcc;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* mImpactParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* mBulletTrace;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* mProjectileMovement;

	UPROPERTY(VisibleDefaultsOnly)
	USphereComponent* mCollisionComp;

private:

	// UI
	FOnHitmark mShowHitmark;

	// Data
	FVector mStartLocation;
	FOnUpdateScore mOnUpdateScore;
	FOnUpdateKill mOnUpdateKill;
	FOnUpdateHeatshotCount mOnUpdateHeadshotCount;


	eWeaponType mBulletType;
	bool mbIsCollision;
};


