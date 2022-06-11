// Fill out your copyright notice in the Description page of Project Settings.


#include "SRTargetManager.h"

#include "SRShortRangeGameMode.h"
#include "SRMidRangeGameMode.h"
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

	// test for checking target movement.
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASRSpawnPoint::StaticClass(), mSpawnPointList);
	UE_LOG(LogTemp, Warning, TEXT("ASRTargetManager : BeginPlay - spawnpoint amount : [ %d ]"), mSpawnPointList.Num());


	for(auto spawner :mSpawnPointList)
	{
		auto castSpawner = Cast<ASRSpawnPoint>(spawner);
	 	switch(castSpawner->GetSpawnPointType())
	 	{
	 		case ESpawnPointType::CharacterType:
				mCharacterSpawnPointList.Add(castSpawner);
				break;
	 		case ESpawnPointType::PlateType:
				mPlateSpawnPointList.Add(castSpawner);
				break;
	 		default:
				UE_LOG(LogTemp, Warning, TEXT("ASRTargetManager : BeginPlay - ESpawnPointType 올바르지 않은 enum 타입입니다."));
	 	}
	}

}

void ASRTargetManager::RandomTargetSpawn()
{
	bool bComplete = false;
	ASRSpawnPoint* spawnPoint = nullptr;

	int32 amountSpawnPoint = mbIsCharacterType ? mCharacterSpawnPointList.Num() - 1 : mPlateSpawnPointList.Num() - 1;
	std::uniform_int_distribution<int> RNG(0, amountSpawnPoint);

	// 해당 스폰 포인트에 타겟이 생성되어 있으면 다른 스폰포인트를 고릅니다.
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

		if (!spawnPoint->IsActive())
		{
			bComplete = true;
		}

	} while (!bComplete);

	FActorSpawnParameters rules;
	rules.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const auto target = GetWorld()->SpawnActor<ASRTargetCharacter>(spawnPoint->GetActorLocation(), spawnPoint->GetActorRotation(), rules);

	target->SetCrouchable(spawnPoint->IsCrouchable());
	target->SetActorRotation(spawnPoint->GetActorRotation());
	target->SetTargetType(mbIsCharacterType);
	target->BindSpawnPoint(spawnPoint);
	target->BindTargetManager(this);
	target->ActiveTarget();
	spawnPoint->Active();

	// spawnPoint와 바인드하여 타겟이 내려갈 때 delegate로 spawnPoint에 Active상태를 지움.
	UE_LOG(LogTemp, Warning, TEXT("ASRTargetManager : RandomTargetActive - Target [%s] is Active : at[%s], "), *target->GetName(), *spawnPoint->GetName());
	switch (spawnPoint->GetSpawnPointType())
	{
	case ESpawnPointType::CharacterType:
		UE_LOG(LogTemp, Warning, TEXT("ASRTargetManager : RandomTargetSpawn - Chracter Type"));
		break;
	case ESpawnPointType::PlateType:
		UE_LOG(LogTemp, Warning, TEXT("ASRTargetManager : RandomTargetSpawn - Plate Type"));
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("ASRTargetManager : RandomTargetSpawn - ESpawnPointType 올바르지 않은 enum 타입입니다."));
	}

	// 타겟 모드가 이동형일 때 처리됩니다.
	if(mbMovableTargetMode)
	{
		target->SetMovable(true);

		// 랜덤으로 지정 범위 내 값만큼 이동 거리를 결정합니다.
		float randomDistance = 0.0f;
		switch(mGameModeType)
		{
			case EGameModeType::Movable_ShortRange:
				randomDistance = FMath::RandRange(150.0f, 200.0f);
				break;
			case EGameModeType::Movable_MidRange:
				randomDistance = FMath::RandRange(150.0f, 250.0f);
				break;
			case EGameModeType::Movable_LongRange:
				randomDistance = FMath::RandRange(200.0f, 400.0f);
				break;
			default:
				UE_LOG(LogTemp, Warning, TEXT("ASRTargetManager : RandomTargetActive - 올바르지 않은 타입이거나 고정형 타겟 모드임에도 실행되었습니다."));
		}

		target->SetEndLocation( randomDistance, spawnPoint->GetMovableAxis());

		target->SetSpeed(FMath::RandRange(1.0f, 2.0f));
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
	default:
		// not movable
		UE_LOG(LogTemp, Warning, TEXT("ASRTargetManager : SetMovableTargetMode - 고정형 타겟 설정."));
		mbMovableTargetMode = false;
	}
}

void ASRTargetManager::SetTargetType(bool isCharacter)
{
	mbIsCharacterType = isCharacter;
}

