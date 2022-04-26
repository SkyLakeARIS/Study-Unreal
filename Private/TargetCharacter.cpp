// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetCharacter.h"

#include "CharacterPlayerController.h"
#include "SRPlayerState.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ATargetCharacter::ATargetCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(42.f, 88.0f);
	
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	mHP =100;
	mHitScore = 100;
	mKillScore = 1000;
}

// Called when the game starts or when spawned
void ATargetCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ATargetCharacter::Dead()
{
	Destroy();
}

// Called every frame
void ATargetCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATargetCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATargetCharacter::BindUpdateScoreAndKillState(ASRPlayerState* srPlayerState)
{
	mOnUpdateScore.BindUObject(srPlayerState, &ASRPlayerState::OnAddScore);
	mOnUpdateKill.BindUObject(srPlayerState, &ASRPlayerState::OnKill);
}

void ATargetCharacter::OnHit(int32 damage)
{
	if(mHP > 0)
	{
		mHP -= damage;
		UE_LOG(LogTemp, Warning, TEXT("target : hit  : %d"), mHP);
		mOnUpdateScore.Execute(mHitScore);
	}

	if(mHP <= 0)
	{
		mOnUpdateScore.Execute(mKillScore);
		mOnUpdateKill.Execute();
		GetCapsuleComponent()->SetCollisionProfileName(FName("Ragdoll"));
		
		GetWorld()->GetTimerManager().SetTimer(mDeathTimer, this, &ATargetCharacter::Dead, 10.0f, false,10.0f);
	}
}

int32 ATargetCharacter::GetHP()
{
	return mHP;
}

