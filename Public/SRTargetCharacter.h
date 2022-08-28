// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "GameFramework/Character.h"
#include "SRTargetCharacter.generated.h"

enum class EMovableAxis : uint8;
class ASRTargetManager;
DECLARE_MULTICAST_DELEGATE(FOnTargetDown)

/*
 * 타겟 클래스입니다.
 * 맵에 배치된 스폰 포인트에서 타겟 매니저에 의해 생성됩니다.
 * 타겟 매니저가 주는 정보대로 초기화됩니다.
 */

UCLASS()
class VERSION_API ASRTargetCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASRTargetCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetTarget(bool isCharacterType, bool isMovable, bool isCrouchable);

	void ActiveTarget();

	UFUNCTION()
	void BindTargetManager(ASRTargetManager* targetManager);

	void BindSpawnPoint(class ASRSpawnPoint* spawnPoint);

	bool OnHit(int32 damage, int32* scoreOut);

	void initializeMovement(FVector endLocation, float speedFactor);

protected:

	virtual void BeginPlay() override;

	void changeCollisionEnabled(ECollisionEnabled::Type newType);

protected:

	// common
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Mesh)
	USkeletalMeshComponent* mMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterType)
	USphereComponent* mHead;

	// character type
	UPROPERTY()
	USkeletalMesh* mCharacterMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterType)
	UAnimSequence* mCharacterStandPoseAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterType)
	UAnimSequence* mCharacterCrouchPoseAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterType)
	UAnimSequence* mCharacterStandDeadAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterType)
	UAnimSequence* mCharacterCrouchDeadAnimation;

	// plate type
	UPROPERTY()
	USkeletalMesh* mPlateMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlateType)
	UAnimSequence* mPlateDownAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlateType)
	UAnimSequence* mPlateUpAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlateType)
	USoundBase* mPlateDownSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlateType)
	USoundBase* mPlateUpSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlateType)
	USoundBase* mPlateHitSound;

private:
	// const
	const int32 MAX_HP = 100;
	const int32 MIN_HP = 0;

	const int32 HIT_SCORE = 100;
	const int32 KILL_SCORE = 1000;

	// target data
	int32 mHP;
	FVector mStartLocation;
	FVector mEndLocation;
	FVector mDistanceToMoveEveryTime;

	// target state
	bool mbIsCharacterType;
	bool mbIsMovable;
	bool mbToEndLocation;
	bool mbIsCrouchable;
	bool mbIsCrouching;

	FOnTargetDown mOnTargetDown;
};


