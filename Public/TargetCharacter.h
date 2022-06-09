// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Version.h"
#include "GameFramework/Character.h"
#include "TargetCharacter.generated.h"


enum class EMovableAxis : uint8;
class ASRTargetManager;
DECLARE_MULTICAST_DELEGATE(FOnTargetDown)

UCLASS()
class VERSION_API ATargetCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATargetCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable)
	void SetTargetType(bool isCharacterType);

	void ActiveTarget();

	UFUNCTION()
	void BindTargetManager(ASRTargetManager* targetManager);

	void BindSpawnPoint(class ASRSpawnPoint* spawnPoint);


	bool OnHit(int32 damage, int32* scoreOut);
	
	UFUNCTION(BlueprintCallable)
	int32 GetHP();

	bool IsActive();

	void SetEndLocation(float distance, EMovableAxis direction);

	void SetMovable(bool isMovable);

	void SetSpeed(float newSpeed);

	void SetCrouchable(bool canCrouch);

protected:

	virtual void BeginPlay() override;

	void dead();

protected:

	// common
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Mesh)
	USkeletalMeshComponent* mMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterType)
	USphereComponent* mHead;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector mStartLocation;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector mEndLocation;

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

	bool mbIsStandPose;


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
	FOnTargetDown mOnTargetDown;
	FTimerHandle mDeathTimer;
	bool mbIsCharacterType;
	bool mbIsDown;
	bool mbIsMovable;
	bool mbIsReturn;
	bool mbCanCrouch;
	FVector mDirection;
	float mSpeedFactor;
	float mDistance;
	int32 mHP;
	int32 mHitScore;
	int32 mKillScore;
};


