// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Version.h"
#include "GameModeData.h"
#include "SRPlayerState.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SRProjectile.generated.h"

class ASRPlayerController;
enum class EWeaponType : uint8;
UENUM(BlueprintType)
enum class EHitType : uint8
{
	Hit,
	HeadShot,
	Kill
};

class USphereComponent;
class UProjectileMovementComponent;

DECLARE_DELEGATE(FOnUpdateHitCount)
DECLARE_DELEGATE(FOnUpdateHeatshotCount)
DECLARE_DELEGATE(FOnUpdateKill)
DECLARE_DELEGATE_OneParam(FOnUpdateScore, int32)
DECLARE_DELEGATE_OneParam(FOnHitmark, EHitType)


/*
 * 각 총알들의 부모 클래스입니다.
 * 충돌처리, 점수처리는 부모에서 처리합니다.
 */

UCLASS(config=Game)
class ASRProjectile : public AActor
{
	GENERATED_BODY()

public:

	ASRProjectile();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void BindPlayerStateInfo(ASRPlayerState* playerState);

	UFUNCTION()
	void BindHUDWidget(UUIHUDWidget* hud);

	UFUNCTION()
	void SetBulletType(EWeaponType gunType);

	void SetStartLocation(FVector location);

	void SetDebugMode(bool active);

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	USphereComponent* GetCollisionComp() const;

	UProjectileMovementComponent* GetProjectileMovement() const;

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

	bool mbDebugMode;
	// UI
	FOnHitmark mShowHitmark;

	// Datas
	FVector mStartLocation;
	FOnUpdateScore mOnUpdateScore;
	FOnUpdateKill mOnUpdateKill;
	FOnUpdateHeatshotCount mOnUpdateHeadshotCount;
	EWeaponType mBulletType;
	EHitType mHitType;
	int32 mBulletDamage;
	bool mbIsCollision;
	bool mbIsHeadshot;
	bool mbIsTargetHit;
};


