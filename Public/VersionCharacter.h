// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Version.h"
#include "GameFramework/Character.h"
#include "VersionCharacter.generated.h"


class UADAnimInstance;
UCLASS(config=Game)
class AVersionCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AVersionCharacter();

	virtual void PossessedBy(AController* NewController) override;

	virtual void PostInitializeComponents() override;
	
	/** Returns Mesh1P subobject **/
	UFUNCTION(BlueprintPure)
		USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UStaticMeshComponent* GetCurrentOptic() const { return CurrentOptic; }

	USkeletalMeshComponent* GetFPGun() const { return FP_Gun; }

	UChildActorComponent* GetScope() const { return mScope; }
protected:
	virtual void BeginPlay() override;

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	UFUNCTION(BlueprintCallable, Category = TUTO)
	void SetAiming(bool IsAiming);

	UFUNCTION(BlueprintCallable, Category = TUTO)
	void CycleOptic();

	UFUNCTION(BlueprintCallable, Category = TUTO)
	void Reload();

	void SwitchFireMode();

	void StartFire();

	void StopFire();

	void BurstFire();

	void FireShot();

	void SwitchMode();

	void SetAim();
	
	void SetHip();
	
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AVersionProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* FireAnimation;
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* ReloadAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UParticleSystem* MuzzleParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UParticleSystem* ImpactParticles;

protected:

	UPROPERTY(BlueprintReadOnly, Category = TUTO)
	UADAnimInstance* TutAnimInstance;

	bool bIsAiming;

	uint8 OpticIndex;

	uint8 ModeIndex;

	uint8 ModeCount;
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Mesh)
		USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
		USkeletalMeshComponent* FP_Gun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
		TArray<UStaticMeshComponent*> Optics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
		UStaticMeshComponent* CurrentOptic;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GUN, meta = (AllowPrivateAccess = "true"))
	USceneComponent* mScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GUN, meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* mScope;
private:


	class ACharacterPlayerController* PlayerController;

	FTimerHandle TimerHandle_BurstMode;
	FTimerHandle TimerHandle_FullAutoMode;

	EWaeponFireMode CurrentFireMode;
	int32 FireModeOffset;
	int32 CurrentBurst;

	int32 FIRE_SWITCH_MODE;
	float FireRate;

	int32 CurrentAmmo;

};

