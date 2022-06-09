// Fill out your copyright notice in the Description page of Project Settings.


#include "UIHUDWidget.h"
#include "Components/TextBlock.h"
#include "VersionCharacter.h"


void UUIHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Accuracy = Cast<UTextBlock>(GetWidgetFromName(TEXT("DisplayAccuracy")));
	RemainingTime = Cast<UTextBlock>(GetWidgetFromName(TEXT("DisplayTimeCountDown")));
	FireMode = Cast<UTextBlock>(GetWidgetFromName(TEXT("DisplayFireMode")));
	RemainingAmmo = Cast<UTextBlock>(GetWidgetFromName(TEXT("DisplayAmmo")));
	Score = Cast<UTextBlock>(GetWidgetFromName(TEXT("DisplayScore")));
	DisplayGameMode = Cast<UTextBlock>(GetWidgetFromName(TEXT("DisplayGameMode")));

}

void UUIHUDWidget::UpdateRemainingTime(int32 remainingTime)
{
	RemainingTime->SetText(FText::FromString(FString::FromInt(remainingTime)));
}

void UUIHUDWidget::UpdateAmmo(int32 newAmmo)
{
	RemainingAmmo->SetText(FText::FromString(FString::FromInt(newAmmo)));
}

void UUIHUDWidget::UpdateAccuracy(int32 accuracy)
{
	Accuracy->SetText(FText::FromString(FString::FromInt(accuracy)+FString("%")));
}

void UUIHUDWidget::UpdateFireMode(EWaeponFireMode newFireMode)
{
	FString DisplayFireMode;
	switch(newFireMode)
	{
		case EWaeponFireMode::SINGLE_FIRE:
			DisplayFireMode = "Single";
			break;
		case EWaeponFireMode::BURST_FIRE:
			DisplayFireMode = "Burst";
			break;
		case EWaeponFireMode::FULL_AUTO:
			DisplayFireMode = "Full auto";
			break;
	}
	FireMode->SetText(FText::FromString(DisplayFireMode));
}

void UUIHUDWidget::UpdateScore(int32 score)
{
	Score->SetText(FText::FromString(FString::FromInt(score)));
}

void UUIHUDWidget::UpdateGameMode(FString newGameMode)
{
	DisplayGameMode->SetText(FText::FromString(newGameMode));
}
