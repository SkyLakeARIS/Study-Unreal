// Copyright Epic Games, Inc. All Rights Reserved.

#include "VersionGameMode.h"
#include "VersionHUD.h"

AVersionGameMode::AVersionGameMode()
//	: Super()
{

}

void AVersionGameMode::SelectButton1()
{
	UE_LOG(LogTemp, Warning, TEXT("selected 1"));
}

void AVersionGameMode::SelectButton2()
{
	UE_LOG(LogTemp, Warning, TEXT("selected2"));

}
