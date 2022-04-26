// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSCharacter.h"
#include "VersionProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterPlayerController.h"
#include "DrawDebugHelpers.h"
#include "TPSAnimInstance.h"
#include "UIHUDWidget.h"

//#include "GameFramework/InputSettings.h"

//////////////////////////////////////////////////////////////////////////
// ATPSCharacter

ATPSCharacter::ATPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	//CharacterStat = CreateDefaultSubobject<UPlayerInfo>(TEXT("CHARACTERSTAT"));



	// Create a CameraComponent	
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	mCamera->SetupAttachment(GetCapsuleComponent());
	mCamera->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	mCamera->bUsePawnControlRotation = true;
	mCamera->SetupAttachment(RootComponent);
	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	mMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	mMesh->SetOnlyOwnerSee(true);
	//mMesh->SetupAttachment(mCamera);
	mMesh->bCastDynamicShadow = false;
	mMesh->CastShadow = false;
	mMesh->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	mMesh->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
	mMesh->SetupAttachment(RootComponent);

	// Create a gun mesh component
	mWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	mWeapon->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	mWeapon->bCastDynamicShadow = false;
	mWeapon->CastShadow = false;
	// mWeapon->SetupAttachment(mMesh, TEXT("GripPoint"));
	mWeapon->SetupAttachment(RootComponent);

	mScene = CreateDefaultSubobject<USceneComponent>(TEXT("ScopeScene"));
	mScene->SetupAttachment(mMesh);


	//mScene->SetupAttachment(mWeapon);
	mScope = CreateDefaultSubobject<UChildActorComponent>(TEXT("ScopeOptic"));
	mScope->SetupAttachment(mScene);
	//mScope->SetChildActorClass();

	//static ConstructorHelpers::FObjectFinder<USceneComponent> A_Scope(TEXT("/Game/Assets/FPS_Weapon_Bundle/Weapons/Meshes/Accessories/BP_Scope2.BP_Scope2"));
	//if(A_Scope.Succeeded())
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("find success"));
	//	mScope = A_Scope.Object;
	//}

	bIsAiming = false;
	OpticIndex = 0;
	FIRE_SWITCH_MODE = 3;
	FireModeOffset = 2;
	CurrentBurst = 0;
	CurrentFireMode = EWaeponFireMode::FULL_AUTO;

	CurrentAmmo = 30;
}

void ATPSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ATPSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	for (auto asd : GetComponents())
	{
		UE_LOG(LogTemp, Warning, TEXT("find %s"), *asd->GetName());
	}

}


void ATPSCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();


	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	//mWeapon->AttachToComponent(mMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("S_HandR"));
	mWeapon->AttachToComponent(mMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("S_HandR"));
	mCamera->AttachToComponent(mMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("Camera"));
	mScope->AttachToComponent(mWeapon, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("S_Sight0"));
	TutAnimInstance = Cast<UTPSAnimInstance>(GetTPSMesh()->GetAnimInstance());
	PlayerController = Cast<ACharacterPlayerController>(GetController());



	EnableInput(PlayerController);

}

//////////////////////////////////////////////////////////////////////////
// Input

void ATPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATPSCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATPSCharacter::StopFire);
	PlayerInputComponent->BindAction("SwitchFireMode", IE_Pressed, this, &ATPSCharacter::SwitchFireMode);
	PlayerInputComponent->BindAction("SwitchOptic", IE_Pressed, this, &ATPSCharacter::CycleOptic);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ATPSCharacter::Reload);

	PlayerInputComponent->BindAction("SwitchMode", IE_Pressed, this, &ATPSCharacter::SwitchMode);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ATPSCharacter::SetAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ATPSCharacter::SetHip);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ATPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPSCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATPSCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATPSCharacter::LookUpAtRate);
}


void ATPSCharacter::SetAiming(bool IsAiming)
{
	bIsAiming = IsAiming;
	if (TutAnimInstance)
	{
		TutAnimInstance->SetAiming(IsAiming);
	}
}

void ATPSCharacter::CycleOptic()
{
	if (bIsAiming)
	{
		if (++OpticIndex >= Optics.Num())
		{
			OpticIndex = 0;
		}
		CurrentOptic = Optics[OpticIndex];
		if (TutAnimInstance)
		{
			TutAnimInstance->CycledOptic();
		}
		UE_LOG(LogTemp, Warning, TEXT("Chaged optic index : %d"), OpticIndex);
	}

}

void ATPSCharacter::Reload()
{
	// try and play a firing animation if specified
	if (FireAnimation)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = mMesh->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(ReloadAnimation, 1.f);
			AnimInstance->Montage_JumpToSection(FName("Reload"));

		}
	}

	if (TutAnimInstance)
	{
		TutAnimInstance->Reload();
		CurrentAmmo = 30;	// anim???? notify ?????? reload complete?? ammo?? ????
		PlayerController->GetIngameHUD()->UpdateAmmo(CurrentAmmo);
	}
}

void ATPSCharacter::SwitchFireMode()
{
	EWaeponFireMode modes[] = { EWaeponFireMode::SINGLE_FIRE, EWaeponFireMode::BURST_FIRE,EWaeponFireMode::FULL_AUTO };
	FireModeOffset = (FireModeOffset + 1) % FIRE_SWITCH_MODE;
	CurrentFireMode = modes[FireModeOffset];
	switch (CurrentFireMode)
	{
	case EWaeponFireMode::SINGLE_FIRE:
		UE_LOG(LogTemp, Warning, TEXT("Switch FireMode Successfully! : Single"));
		break;
	case EWaeponFireMode::BURST_FIRE:
		UE_LOG(LogTemp, Warning, TEXT("Switch FireMode Successfully! : Burst"));
		break;
	case EWaeponFireMode::FULL_AUTO:
		UE_LOG(LogTemp, Warning, TEXT("Switch FireMode Successfully! : Full Auto"));
		break;
	}
	PlayerController->GetIngameHUD()->UpdateFireMode(CurrentFireMode);
}

void ATPSCharacter::StartFire()
{
	if (CurrentAmmo > 0)
	{
		switch (CurrentFireMode)
		{
		case EWaeponFireMode::SINGLE_FIRE:
			FireShot();
			break;
		case EWaeponFireMode::BURST_FIRE:
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_BurstMode, this, &ATPSCharacter::BurstFire, 0.1f, true, 0.0f);
			break;
		case EWaeponFireMode::FULL_AUTO:
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_FullAutoMode, this, &ATPSCharacter::FireShot, 0.10f, true, 0.0f);
			break;

		}
		if (FireAnimation != NULL)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = GetTPSMesh()->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}

		}
	}
	else
	{
		Reload();
	}

}

void ATPSCharacter::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_FullAutoMode);
}

void ATPSCharacter::BurstFire()
{
	if (CurrentBurst < 3) {
		FireShot();
		CurrentBurst++;
	}
	else {
		CurrentBurst = 0;
		GetWorldTimerManager().ClearTimer(TimerHandle_BurstMode);
	}
}

void ATPSCharacter::FireShot()
{

	/*
	 * modeIndex
	 *  0 is bf mode (projectile, camera(eyes)
	 *	1 is r6 mode ( hitscan, camera eyes)
	 *	2 is  tarkov ( projectile, muzzle)
	 */

	--CurrentAmmo;
	if (CurrentAmmo < 0)
	{
		CurrentAmmo = 0;
		return;
	}
	if (MuzzleParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleParticles, mScope->GetSocketTransform(FName("S_Muzzle")).GetLocation());
	}
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	PlayerController->GetPlayerState()->OnAddFireShots(1);
	TutAnimInstance->Fire();

	FVector MuzzleLocation;
	FVector EndTrace;
	FCollisionQueryParams QueryParams = FCollisionQueryParams(SCENE_QUERY_STAT(WeaponTrace), false, this);
	FHitResult Hit;
	AVersionProjectile* projectile = nullptr;
	switch (ModeIndex)
	{
	case 0:
	{
		//recoil
		// ?????? ?ð??? ????????? ?????? ?? Ŀ????
		// ??? ???? ?????? ???? ????
		FVector randRecoil(FMath::RandRange(-10.0f, 10.0f), FMath::RandRange(-10.0f, 10.0f), FMath::RandRange(-10.0f, 10.0f));

		MuzzleLocation = mCamera->GetComponentLocation() + (mCamera->GetForwardVector() * 100.0f);
		//EndTrace = (mCamera->GetForwardVector() * 10000.0f) + MuzzleLocation + randRecoil;	// 100????
		//DrawDebugLine(GetWorld(), MuzzleLocation, EndTrace, FColor::Green, false, 10.0f);
		MuzzleLocation += randRecoil;
		projectile = GetWorld()->SpawnActor<AVersionProjectile>(ProjectileClass, MuzzleLocation, GetControlRotation());
		break;
	}
	case 1:
	{
		FVector randRecoil(FMath::RandRange(-10.0f, 10.0f), FMath::RandRange(-50.0f, 50.0f), FMath::RandRange(-50.0f, 50.0f));
		MuzzleLocation = mCamera->GetComponentLocation();
		EndTrace = (mCamera->GetForwardVector() * 10000.0f) + MuzzleLocation + randRecoil;

		//GetWorld()->LineTraceSingleByChannel(Hit, MuzzleLocation, EndTrace, ECC_Visibility, QueryParams);
		if (GetWorld()->LineTraceSingleByChannel(Hit, MuzzleLocation, EndTrace, ECC_Visibility, QueryParams))
		{
			//if (ImpactParticles)
			//{
			//	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint));
			//}

			projectile = GetWorld()->SpawnActor<AVersionProjectile>(ProjectileClass, Hit.Location, Hit.GetComponent()->GetRelativeRotation());
			projectile->SetLifeSpan(1.0f);

			DrawDebugLine(GetWorld(), MuzzleLocation, Hit.ImpactPoint, FColor::Red, false, 10.0f);
		}
		break;
	}
	case 2:
	{
		FVector MuzzleLoc = mScope->GetSocketTransform(FName("S_Muzzle")).GetLocation();
		FVector MuzzleOriginLoc = mScope->GetSocketTransform(FName("S_MuzzleOrigin")).GetLocation();

		MuzzleLocation = MuzzleLoc + ((MuzzleLoc - MuzzleOriginLoc) * 0.3f);
		FRotator MuzzleRotation = mScope->GetSocketTransform(FName("S_Muzzle")).GetRotation().Rotator();
		UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), mScope->GetForwardVector().X, mScope->GetForwardVector().Y, mScope->GetForwardVector().Z);

		projectile = GetWorld()->SpawnActor<AVersionProjectile>(ProjectileClass, MuzzleLocation, GetControlRotation());

		break;
	}
	}
	// R6S 방식에서 ray가 아무것도 충돌을 감지하지 못하면 projectile이 생성되지 않으므로
	// nullptr가 발생함으로 허공에 발사시에 대한 안전장치
	if (projectile != nullptr)
	{
		projectile->BindHitCheck(PlayerController->GetPlayerState());
	}
	PlayerController->GetIngameHUD()->UpdateAmmo(CurrentAmmo);
}

void ATPSCharacter::SwitchMode()
{

	/*
	 * modeIndex
	 *  0 is bf mode (projectile, camera(eyes)
	 *	1 is r6 mode ( hitscan, camera eyes)
	 *	2 is  tarkov ( projectile, muzzle)
	 */
	ModeCount = 3;
	ModeIndex = (ModeIndex + 1) % ModeCount;
	FString DisplayMode;
	switch (ModeIndex)
	{
	case 0:
		UE_LOG(LogTemp, Warning, TEXT("GameMode : battlefield mode is applied"));
		DisplayMode = " battlefield mode";
		break;
	case 1:
		UE_LOG(LogTemp, Warning, TEXT("GameMode : rainbow six:siege mode is applied"));
		DisplayMode = "rainbow six:siege mode";

		break;
	case 2:
		UE_LOG(LogTemp, Warning, TEXT("GameMode : Tarkov mode is applied"));
		DisplayMode = "Tarkov mode";

		break;
	}

	PlayerController->GetIngameHUD()->UpdateGameMode(DisplayMode);
}

void ATPSCharacter::SetAim()
{
	bIsAiming = true;
	TutAnimInstance->SetAiming(true);
}

void ATPSCharacter::SetHip()
{
	bIsAiming = false;
	TutAnimInstance->SetAiming(false);
}

void ATPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ATPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ATPSCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATPSCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}