// Fill out your copyright notice in the Description page of Project Settings.
#include "SRTargetManager.h"
#include "SRGameMode.h"
#include "SRSpawnPoint.h"
#include "SRTargetCharacter.h"

// Sets default values
ASRTargetManager::ASRTargetManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	mbMovableTargetMode = false;
	// random_device 를 통해 난수 생성 엔진을 초기화 합니다.
	mGen.seed(mRandomDevice());
}

// Called when the game starts or when spawned
void ASRTargetManager::BeginPlay()
{
	Super::BeginPlay();

	// 레벨에 배치된 모든 스폰 포인트 리스트로 가져옵니다.
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASRSpawnPoint::StaticClass(), mSpawnPointList);
	UE_LOG(LogTemp, Warning, TEXT("ASRTargetManager : BeginPlay - spawnpoint amount : [ %d ]"), mSpawnPointList.Num());

	for(auto* spawner :mSpawnPointList)
	{
		auto* castedSpawner = Cast<ASRSpawnPoint>(spawner);
	 	switch(castedSpawner->GetSpawnPointType())
	 	{
	 		case ESpawnPointType::CharacterType:
				mCharacterSpawnPointList.Add(castedSpawner);
				break;
	 		case ESpawnPointType::PlateType:
				mPlateSpawnPointList.Add(castedSpawner);
				break;
	 		default:
				checkf(false, TEXT("ASRTargetManager : BeginPlay - ESpawnPointType 올바르지 않은 enum 타입입니다."));
				break;
	 	}
	}
}

void ASRTargetManager::RandomTargetSpawn()
{
	const int32 amountSpawnPoint = mbIsCharacterType ? mCharacterSpawnPointList.Num() - 1 : mPlateSpawnPointList.Num() - 1;
	const std::uniform_int_distribution<int> RNG(0, amountSpawnPoint);

	// 해당 스폰 포인트에 타겟이 생성되어 있으면 다른 스폰포인트를 고릅니다.
	ASRSpawnPoint* spawnPoint = nullptr;
	do
	{
		if(mbIsCharacterType)
		{
			spawnPoint = Cast<ASRSpawnPoint>(mCharacterSpawnPointList[RNG(mGen)]);
		}
		else
		{
			spawnPoint = Cast<ASRSpawnPoint>(mPlateSpawnPointList[RNG(mGen)]);
		}

	} while (spawnPoint->IsActive());

	FActorSpawnParameters rules;
	rules.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto* target = GetWorld()->SpawnActor<ASRTargetCharacter>(spawnPoint->GetActorLocation(), spawnPoint->GetActorRotation(), rules);

	// 기본적인 타겟 데이터를 설정합니다.
	target->SetActorRotation(spawnPoint->GetActorRotation());
	target->SetTarget(mbIsCharacterType, mbMovableTargetMode, spawnPoint->IsCrouchable());
	// 타겟이 다운되었을 때 delegate 실행을 위해 바인드합니다.
	target->BindSpawnPoint(spawnPoint);
	target->BindTargetManager(this);
	// 타겟을 활성화 시킵니다. (충돌 처리 가능)
	target->ActiveTarget();
	spawnPoint->Active();

	// 타겟 모드가 이동형일 때 처리됩니다.
	if(mbMovableTargetMode)
	{
		// 랜덤으로 지정 범위 내 값만큼 이동 거리를 결정합니다.
		float distanceToMove = 0.0f;
		switch(mGameModeType)
		{
			case EGameModeType::Movable_ShortRange:
				distanceToMove = FMath::RandRange(150.0f, 200.0f);
				break;
			case EGameModeType::Movable_MidRange:
				distanceToMove = FMath::RandRange(150.0f, 250.0f);
				break;
			case EGameModeType::Movable_LongRange:
				distanceToMove = FMath::RandRange(200.0f, 400.0f);
				break;
			default:
				checkf(false, TEXT("ASRTargetManager : RandomTargetActive - EGameModeType 올바르지 않은 타입이거나 고정형 타겟 모드임에도 실행되었습니다."));
				break;
		}

		FVector endLocation = spawnPoint->GetActorLocation();
		switch(spawnPoint->GetMovableAxis())
		{
			case EMovableAxis::X:
				endLocation.X += distanceToMove;
				break;
			case EMovableAxis::Y:
				endLocation.Y += distanceToMove;
				break;
			default:
				checkf(false, TEXT("ASRTargetManager : RandomTargetActive - EMovableAxis 올바르지 않은 타입이거나 고정형 타겟 모드임에도 실행되었습니다."));
			break;
		}

		target->initializeMovement(endLocation, FMath::RandRange(1.0f, 2.0f));
	}
}

void ASRTargetManager::SetMovableTargetMode(EGameModeType mode)
{
	mGameModeType = mode;

	switch (mGameModeType)
	{
		case EGameModeType::Movable_ShortRange:
		case EGameModeType::Movable_MidRange:
		case EGameModeType::Movable_LongRange:
			mbMovableTargetMode = true;
			UE_LOG(LogTemp, Warning, TEXT("ASRTargetManager : SetMovableTargetMode - 이동형 타겟 설정."));
			break;
		case EGameModeType::Static_ShortRange:
		case EGameModeType::Static_MidRange:
		case EGameModeType::Static_LongRange:
			mbMovableTargetMode = false;
			UE_LOG(LogTemp, Warning, TEXT("ASRTargetManager : SetMovableTargetMode - 고정형 타겟 설정."));
			break;
		default:
			checkf(false, TEXT("ASRTargetManager : SetMovableTargetMode - Static or Movable 타입의 형식이 되어야합니다."));
			break;
	}
}

void ASRTargetManager::SetTargetType(bool isCharacter)
{
	mbIsCharacterType = isCharacter;
}

EGameModeType ASRTargetManager::GetGameModeType() const
{
	return mGameModeType;
}

