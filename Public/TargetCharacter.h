// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TargetCharacter.generated.h"

class ASRPlayerState;

DECLARE_DELEGATE_OneParam(FOnUpdateScore, int32)
DECLARE_DELEGATE(FOnUpdateKill)

UCLASS()
class VERSION_API ATargetCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATargetCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//virtual void PostInitializeComponents() override;

	UFUNCTION()
	void BindUpdateScoreAndKillState(ASRPlayerState* srPlayerState);

	UFUNCTION(BlueprintCallable)
	void OnHit(int32 damage);
	
	UFUNCTION(BlueprintCallable)
	int32 GetHP();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;
protected:
	
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* mMesh;

	void Dead();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* mDeadAnimation;

private:
	FOnUpdateScore mOnUpdateScore;
	FOnUpdateKill mOnUpdateKill;
	FTimerHandle mDeathTimer;
	int32 mHP;
	int32 mHitScore;
	int32 mKillScore;
};

