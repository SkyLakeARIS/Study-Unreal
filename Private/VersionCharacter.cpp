// Copyright Epic Games, Inc. All Rights Reserved.

#include "VersionCharacter.h"

#include "ADAnimInstance.h"
#include "VersionProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterPlayerController.h"
#include "DrawDebugHelpers.h"
#include "UIHUDWidget.h"

//#include "GameFramework/InputSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AVersionCharacter

AVersionCharacter::AVersionCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	//CharacterStat = CreateDefaultSubobject<UPlayerInfo>(TEXT("CHARACTERSTAT"));



	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	mScene = CreateDefaultSubobject<USceneComponent>(TEXT("ScopeScene"));
	mScene->SetupAttachment(RootComponent);


	//mScene->SetupAttachment(FP_Gun);
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

void AVersionCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void AVersionCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	for(auto asd : GetComponents())
	{
		UE_LOG(LogTemp, Warning, TEXT("find %s"), *asd->GetName());
	}
//	auto aa = Cast<UChildActorComponent>(GetComponents()[12]);
////	Optics.Add(Cast<UStaticMeshComponent>(aa->GetChildActor()->GetComponents().Array()[5]));

	//if(mScope->GetChildActorClass() != nullptr)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("find success"));
	//	//mScope->GetChildActor()->AttachToComponent(FP_Gun, FAttachmentTransformRules::SnapToTargetIncludingScale,FName("S_Sight0"));
	//	//Optics.Add(mScope->GetChildActor()->GetComponents());
	///*	for(auto asd : mScope->GetChildActor()->GetComponents().Array())
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("find %s"), *asd->GetName());
	//	}*/
	//	auto a = Cast<UStaticMeshComponent>(mScope->GetChildActor()->GetComponents().Array()[5]);
	//	UE_LOG(LogTemp, Warning, TEXT("mesh is : %s"), *a->GetStaticMesh()->GetName());
	//	//a->AttachToComponent(FP_Gun, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("S_Sight0"));
	//	Optics.Add(a);
	//	mScope->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
	//}
}


void AVersionCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();


	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("S_HandR"));
//	mScope->AttachToComponent(mS, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("S_Sight0"));
	TutAnimInstance = Cast<UADAnimInstance>(GetMesh1P()->GetAnimInstance());
	PlayerController = Cast<ACharacterPlayerController>(GetController());



	EnableInput(PlayerController);

}

//////////////////////////////////////////////////////////////////////////
// Input

void AVersionCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AVersionCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AVersionCharacter::StopFire);
	PlayerInputComponent->BindAction("SwitchFireMode", IE_Pressed, this, &AVersionCharacter::SwitchFireMode);
	PlayerInputComponent->BindAction("SwitchOptic", IE_Pressed, this, &AVersionCharacter::CycleOptic);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AVersionCharacter::Reload);

	PlayerInputComponent->BindAction("SwitchMode", IE_Pressed, this, &AVersionCharacter::SwitchMode);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AVersionCharacter::SetAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AVersionCharacter::SetHip);
	
	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AVersionCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVersionCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AVersionCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AVersionCharacter::LookUpAtRate);
}


void AVersionCharacter::SetAiming(bool IsAiming)
{
	bIsAiming = IsAiming;
	if(TutAnimInstance)
	{
		TutAnimInstance->SetAiming(IsAiming);
	}
}

void AVersionCharacter::CycleOptic()
{
	if(bIsAiming)
	{
		if(++OpticIndex>=Optics.Num())
		{
			OpticIndex = 0;
		}
		CurrentOptic = Optics[OpticIndex];
		if(TutAnimInstance)
		{
			TutAnimInstance->CycledOptic();
		}
		UE_LOG(LogTemp, Warning, TEXT("Chaged optic index : %d"), OpticIndex);
	}

}

void AVersionCharacter::Reload()
{
	// try and play a firing animation if specified
	if (FireAnimation)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(ReloadAnimation, 1.f);
			AnimInstance->Montage_JumpToSection(FName("Reload"));

		}
	}

	if(TutAnimInstance)
	{
		TutAnimInstance->Reload();
		CurrentAmmo = 30;	// anim���� notify �߰��ϰ� reload complete�� ammo�� ����
		PlayerController->GetIngameHUD()->UpdateAmmo(CurrentAmmo);
	}
}

void AVersionCharacter::SwitchFireMode()
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

void AVersionCharacter::StartFire()
{
	if(CurrentAmmo > 0)
	{
		switch (CurrentFireMode)
		{
		case EWaeponFireMode::SINGLE_FIRE:
			FireShot();
			break;
		case EWaeponFireMode::BURST_FIRE:
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_BurstMode, this, &AVersionCharacter::BurstFire, 0.1f, true, 0.0f);
			break;
		case EWaeponFireMode::FULL_AUTO:
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_FullAutoMode, this, &AVersionCharacter::FireShot, 0.10f, true, 0.0f);
			break;

		}
		if (FireAnimation != NULL)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
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

void AVersionCharacter::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_FullAutoMode);
}

void AVersionCharacter::BurstFire()
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

void AVersionCharacter::FireShot()
{

	/*
	 * modeIndex
	 *  0 is bf mode (projectile, camera(eyes)
	 *	1 is r6 mode ( hitscan, camera eyes)
	 *	2 is  tarkov ( projectile, muzzle)
	 */

	--CurrentAmmo;
	if(CurrentAmmo < 0)
	{
		CurrentAmmo = 0;
		return;
	}
	if (MuzzleParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleParticles, FP_Gun->GetSocketTransform(FName("S_Muzzle")).GetLocation());
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
	switch(ModeIndex)
	{
		case 0:
		{
			//recoil
			// ����ϴ� �ð��� ��������� ������ �� Ŀ����
			// Ȥ�� ī�޶� �պ��͸� ���� ����
			FVector randRecoil(FMath::RandRange(-10.0f, 10.0f), FMath::RandRange(-10.0f, 10.0f), FMath::RandRange(-10.0f, 10.0f));

			MuzzleLocation = FirstPersonCameraComponent->GetComponentLocation() + (FirstPersonCameraComponent->GetForwardVector()*100.0f);
			//EndTrace = (FirstPersonCameraComponent->GetForwardVector() * 10000.0f) + MuzzleLocation + randRecoil;	// 100����
			//DrawDebugLine(GetWorld(), MuzzleLocation, EndTrace, FColor::Green, false, 10.0f);
			MuzzleLocation += randRecoil;
			projectile = GetWorld()->SpawnActor<AVersionProjectile>(ProjectileClass, MuzzleLocation, GetControlRotation());
			break;
		}
		case 1:
		{
			FVector randRecoil(FMath::RandRange(-10.0f, 10.0f), FMath::RandRange(-50.0f, 50.0f), FMath::RandRange(-50.0f, 50.0f));
			MuzzleLocation = FirstPersonCameraComponent->GetComponentLocation();
			EndTrace = (FirstPersonCameraComponent->GetForwardVector() * 10000.0f) + MuzzleLocation + randRecoil;

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
			FVector MuzzleLoc = FP_Gun->GetSocketTransform(FName("S_Muzzle")).GetLocation();
			FVector MuzzleOriginLoc = FP_Gun->GetSocketTransform(FName("S_MuzzleOrigin")).GetLocation();

			MuzzleLocation = MuzzleLoc + ((MuzzleLoc - MuzzleOriginLoc)*0.3f);
			FRotator MuzzleRotation = FP_Gun->GetSocketTransform(FName("S_Muzzle")).GetRotation().Rotator();
			UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), FP_Gun->GetForwardVector().X, FP_Gun->GetForwardVector().Y, FP_Gun->GetForwardVector().Z);

			projectile = GetWorld()->SpawnActor<AVersionProjectile>(ProjectileClass, MuzzleLocation, GetControlRotation());

			break;
		}
	}
	// R6S 방식에서 ray가 아무것도 충돌을 감지하지 못하면 projectile이 생성되지 않으므로
	// nullptr가 발생함으로 허공에 발사시에 대한 안전장치
	if(projectile != nullptr)
	{
		projectile->BindHitCheck(PlayerController->GetPlayerState());
	}
	PlayerController->GetIngameHUD()->UpdateAmmo(CurrentAmmo);
}

void AVersionCharacter::SwitchMode()
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
	switch(ModeIndex)
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

void AVersionCharacter::SetAim()
{
	bIsAiming = true;
	TutAnimInstance->SetAiming(true);
}

void AVersionCharacter::SetHip()
{
	bIsAiming = false;
	TutAnimInstance->SetAiming(false);
}

void AVersionCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AVersionCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AVersionCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AVersionCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}