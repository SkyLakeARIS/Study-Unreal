// Fill out your copyright notice in the Description page of Project Settings.
#include "UIResultWidget.h"
#include "SRPlayerController.h"
#include "SRPlayerState.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"


void UUIResultWidget::UpdateStageInfo(const class ASRPlayerState* const srPlayerState)
{
	checkf(srPlayerState != nullptr, TEXT("UUIResultWidget-UpdateStageInfo : srPlayerState이 nullptr입니다."));
	mScore->SetText(FText::FromString(FString::FromInt(srPlayerState->GetScore())));
	mAccuracy->SetText(FText::FromString(FString::FromInt(srPlayerState->GetAccuracy()) + FString("%")));
	mKill->SetText(FText::FromString(FString::FromInt(srPlayerState->GetKillCount())));
}

void UUIResultWidget::NativeConstruct()
{
	Super::NativeConstruct();
	mBtnRetry = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Retry")));
	mBtnStage = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Stage")));
	mBtnMenu= Cast<UButton>(GetWidgetFromName(TEXT("Btn_Menu")));
	mScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("DisplayScore")));
	mAccuracy = Cast<UTextBlock>(GetWidgetFromName(TEXT("DisplayAccuracy")));
	mKill = Cast<UTextBlock>(GetWidgetFromName(TEXT("DisplayKill")));

	mBtnRetry->OnClicked.AddDynamic(this, &UUIResultWidget::onRetryClicked);
	mBtnStage->OnClicked.AddDynamic(this, &UUIResultWidget::onStageClicked);
	mBtnMenu->OnClicked.AddDynamic(this, &UUIResultWidget::onMenuClicked);
}

void UUIResultWidget::onRetryClicked()
{
	const auto playerController = Cast<ASRPlayerController>(GetOwningPlayer());
	playerController->RestartLevel();
}

void UUIResultWidget::onStageClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Stages"));
}

void UUIResultWidget::onMenuClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMenu"));
}
