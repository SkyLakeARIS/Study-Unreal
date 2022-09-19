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

// crosshair, hitmark ������ �ʱ�ȭ�ϰ� ����Ʈ�� �߰��մϴ�.
void UUIHUDWidget::InitializeWidgets()
{
	mHitMark = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), mHitMarkClass);
	mHeadshotMark = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), mHeadshotMarkClass);
	mKillMark = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), mKillMarkClass);
	mCrossHair = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), mCrossHairClass);

	mHitMark->AddToViewport(1);
	mHeadshotMark->AddToViewport(1);
	mKillMark->AddToViewport(1);
	mCrossHair->AddToViewport(1);

	mHitMark->SetVisibility(ESlateVisibility::Hidden);
	mHeadshotMark->SetVisibility(ESlateVisibility::Hidden);
	mKillMark->SetVisibility(ESlateVisibility::Hidden);
	mCrossHair->SetVisibility(ESlateVisibility::Hidden);

	// ������ ������ �����صӴϴ�.
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
		default:
			checkf(false, TEXT("UUIHUDWidget - UpdateFireMode : �ùٸ��� ���� EWaeponFireMode Ÿ���Դϴ�."));
			break;
	}
	mFireMode->SetText(FText::FromString(DisplayFireMode));
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
	if(option == ESlateVisibility::Hidden || option == ESlateVisibility::Visible)
	{
		mCrossHair->SetVisibility(option);
	}
}


// Ÿ�ٿ� ���߽� ������ ��Ʈ��ũ�� ũ�ν��� ��ġ�ϴ� ���� ǥ���ϴ� �Լ��Դϴ�.
// ǥ���� ��Ʈ��ũ�� ������ projectile ��ü�� �������ݴϴ�.
void UUIHUDWidget::AddViewPortHitMark(EHitType hitType)
{
	// ������ ������ ��Ʈ��ũ�� ����Ʈ���� ����ϴ�.
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
			checkf(false, TEXT("UUIHUDWidget - AddViewPortHitMark : �ùٸ��� ���� EHitType Ÿ���Դϴ�."));
			break;
	}

	mCurrentMark->SetVisibility(ESlateVisibility::Visible);

	GetWorld()->GetTimerManager().SetTimer(mHitMarkTimer, this, &UUIHUDWidget::clearHitMark, 0.5f, false, -1.0f);
}

// Ÿ�̸ӿ� ���� ȣ��Ǵ� ��Ʈ��ũ�� ����� �Լ��Դϴ�.
void UUIHUDWidget::clearHitMark()
{
	mCurrentMark->SetVisibility(ESlateVisibility::Hidden);

	GetWorld()->GetTimerManager().ClearTimer(mHitMarkTimer);
}
