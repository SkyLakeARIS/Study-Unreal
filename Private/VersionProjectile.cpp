// Copyright Epic Games, Inc. All Rights Reserved.

#include "VersionProjectile.h"

#include "CharacterPlayerController.h"
#include "DrawDebugHelpers.h"
#include "TargetCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

AVersionProjectile::AVersionProjectile() 
{
	PrimaryActorTick.bCanEverTick = true;
	//FBodyInstance::GetSimplePhysicalMaterial
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(1.0f);
	CollisionComp->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AVersionProjectile::OnHit);		// set up a notification for when this component hits something blocking
	//CollisionComp->SetAllMassScale(10);
	// Players can't walk on it
	//CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Default, 0.f));
	//CollisionComp->CanCharacterStepUpOn = ECB_Yes;
	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 40000.f;
	ProjectileMovement->MaxSpeed = 360000.f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
	// Die after 3 seconds by default
	InitialLifeSpan = 10.0f;
	bIsCollision = false;
}

void AVersionProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsCollision == false)
	{
		DrawDebugPoint(GetWorld(), GetActorLocation(), 3.0f, FColor::Green, false, 20.0f);
		//UE_LOG(LogTemp, Error, TEXT("t speed %f  %f  %f"), ProjectileMovement->Velocity.X, ProjectileMovement->Velocity.Y, ProjectileMovement->Velocity.Z);
	}
}

FOnHitTarget* AVersionProjectile::GetHitDelegate()
{
	return &onHitAndUpdateAcc;
}

void AVersionProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AVersionProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	bIsCollision = true;

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint));
	}

	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 10.0f, GetActorLocation());
	}
	
	if(Hit.GetActor()->GetName().Contains(FString("BP_TargetCharacter")))
	{
		auto targetCharacter = Cast<ATargetCharacter>(Hit.GetActor());
		// 바인드할 마땅한 위치를 아직 알지 못함.
		// beginPlay 나 PostInit 함수는 불가능했음.
		// 따라서 projectile에서 playerstate와 target을
		// 얻어올 수 있으므로 여기에서 바인딩 함수를 호출.
		auto playerController = Cast<ACharacterPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		targetCharacter->BindUpdateScoreAndKillState(playerController->GetPlayerState());

		targetCharacter->OnHit(30);
		onHitAndUpdateAcc.Execute();
	}
	//UE_LOG(LogTemp, Warning, TEXT("opther comp get name : %s"), *Hit.Component->GetName());
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *OtherComp->getname());

	Destroy();
}

void AVersionProjectile::BindHitCheck(ASRPlayerState* playerState)
{
	onHitAndUpdateAcc.BindUObject(playerState, &ASRPlayerState::OnHitCount);
}
