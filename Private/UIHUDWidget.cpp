// Fill out your copyright notice in the Description page of Project Settings.
#include "UIHUDWidget.h"
#include "Components/TextBlock.h"
#include "SRPlayerCharacter.h"
#include "SRProjectile.h"

void UUIHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	mAccuracy = Cast<UTextBlock>(GetWidgetFromName(TEXT("DisplayAccuracy")));
	mRemainingTime = Cast<UTextBlock>(GetWidgetFromName(TEXT("DisplayTimeCountDown")));
	mFireMode = Cast<UTextBlock>(GetWidgetFromName(TEXT("DisplayFireMode")));
	mRemainingAmmo = Cast<UTextBlock>(GetWidgetFromName(TEXT("DisplayAmmo")));
	mScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("DisplayScore")));
	mDisplayGameMode = Cast<UTextBlock>(GetWidgetFromName(TEXT("DisplayGameMode")));
}

// crosshair, hitmark 위젯을 초기화하고 뷰포트에 추가합니다.
void UUIHUDWidget::InitializeWidgets()
{
	auto* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	mHitMark = CreateWidget<UUserWidget>(playerController, mHitMarkClass);
	mHeadshotMark = CreateWidget<UUserWidget>(playerController, mHeadshotMarkClass);
	mKillMark = CreateWidget<UUserWidget>(playerController, mKillMarkClass);
	mCrossHair = CreateWidget<UUserWidget>(playerController, mCrossHairClass);

	mHitMark->AddToViewport(1);
	mHeadshotMark->AddToViewport(1);
	mKillMark->AddToViewport(1);
	mCrossHair->AddToViewport(1);

	mHitMark->SetVisibility(ESlateVisibility::Hidden);
	mHeadshotMark->SetVisibility(ESlateVisibility::Hidden);
	mKillMark->SetVisibility(ESlateVisibility::Hidden);
	mCrossHair->SetVisibility(ESlateVisibility::Hidden);

	// 임의의 값으로 지정해둡니다.
	mCurrentMark = mHitMark;
}

void UUIHUDWidget::UpdateRemainingTime(int32 remainingTime)
{
	mRemainingTime->SetText(FText::FromString(FString::FromInt(remainingTime)));
}

void UUIHUDWidget::UpdateAmmo(int32 newAmmo)
{
	mRemainingAmmo->SetText(FText::FromString(FString::FromInt(newAmmo)));
}

void UUIHUDWidget::UpdateAccuracy(int32 accuracy)
{
	mAccuracy->SetText(FText::FromString(FString::FromInt(accuracy)+FString("%")));
}

void UUIHUDWidget::UpdateFireMode(eWaeponFireMode newFireMode)
{
	FString displayFireMode;
	switch(newFireMode)
	{
		case eWaeponFireMode::SINGLE_FIRE:
			displayFireMode = "Single";
			break;
		case eWaeponFireMode::BURST_FIRE:
			displayFireMode = "Burst";
			break;
		case eWaeponFireMode::FULL_AUTO:
			displayFireMode = "Full auto";
			break;
		default:
			checkf(false, TEXT("UUIHUDWidget - UpdateFireMode : 올바르지 않은 eWaeponFireMode 타입입니다."));
			break;
	}
	mFireMode->SetText(FText::FromString(displayFireMode));
}

void UUIHUDWidget::UpdateScore(int32 score)
{
	mScore->SetText(FText::FromString(FString::FromInt(score)));
}

void UUIHUDWidget::UpdateGameMode(FString newGameMode)
{
	mDisplayGameMode->SetText(FText::FromString(newGameMode));
}

void UUIHUDWidget::SetCrosshairVisibility(ESlateVisibility option)
{
	checkf(option == ESlateVisibility::Hidden || option == ESlateVisibility::Visible, TEXT("UUIHUDWidget - UpdateFireMode : Hidden 또는 Visible 타입만 전달할 수 있습니다."));

	mCrossHair->SetVisibility(option);
}


// 타겟에 적중시 적절한 히트마크를 크로스헤어가 위치하는 곳에 표시하는 함수입니다.
// 표시할 히트마크의 유형은 projectile 개체가 전달해줍니다.
void UUIHUDWidget::AddHitMarkToViewPort(EHitType hitType)
{
	// 이전에 보여진 히트마크를 뷰포트에서 숨깁니다.
	mCurrentMark->SetVisibility(ESlateVisibility::Hidden);

	switch (hitType)
	{
		case EHitType::Hit:
			mCurrentMark = mHitMark;
			break;
		case EHitType::HeadShot:
			mCurrentMark = mHeadshotMark;
			break;
		case EHitType::Kill:
			mCurrentMark = mKillMark;
			break;
		default:
			checkf(false, TEXT("UUIHUDWidget - AddViewPortHitMark : 올바르지 않은 EHitType 타입입니다."));
			break;
	}

	mCurrentMark->SetVisibility(ESlateVisibility::Visible);

	GetWorld()->GetTimerManager().SetTimer(mHitMarkTimer, this, &UUIHUDWidget::hideHitMark, 0.5f, false, -1.0f);
}

// 타이머에 의해 호출되는 히트마크를 지우는 함수입니다.
void UUIHUDWidget::hideHitMark()
{
	mCurrentMark->SetVisibility(ESlateVisibility::Hidden);

	GetWorld()->GetTimerManager().ClearTimer(mHitMarkTimer);
}

void UUIHUDWidget::RemoveHitMarkFromViewport()
{
	mHitMark->RemoveFromParent();
	mHeadshotMark->RemoveFromParent();
	mKillMark->RemoveFromParent();
	mCrossHair->RemoveFromParent();
}
