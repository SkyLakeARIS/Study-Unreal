// Fill out your copyright notice in the Description page of Project Settings.
#include "UIHUDWidget.h"
#include "Components/TextBlock.h"
#include "SRPlayerCharacter.h"
#include "SRProjectile.h"

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
	// �����ͷ� �����Ͽ� �ڵ� ���̱�.
	mCurrentMark = hitType;
	// ���ο� ��Ʈ��ũ�� ǥ���ϱ� ���� �ϴ� ��� ��Ʈ��ũ�� ����Ʈ���� ����ϴ�.
	mHitMark->SetVisibility(ESlateVisibility::Hidden);
	mHeadshotMark->SetVisibility(ESlateVisibility::Hidden);
	mKillMark->SetVisibility(ESlateVisibility::Hidden);

	switch (mCurrentMark)
	{
	case EHitType::Hit:
		mHitMark->SetVisibility(ESlateVisibility::Visible);

		break;
	case EHitType::HeadShot:
		mHeadshotMark->SetVisibility(ESlateVisibility::Visible);
		break;
		case EHitType::Kill:
		mKillMark->SetVisibility(ESlateVisibility::Visible);
		break;
	default:
		UE_LOG(LogTemp, Display, TEXT("UUIHUDWidget - AddViewPortHitMark : �ùٸ��� ���� EHitType Ÿ���Դϴ�."));
		break;
	}
	GetWorld()->GetTimerManager().SetTimer(mHitMarkTimer, this, &UUIHUDWidget::clearHitMark, 0.5f, false, -1.0f);
}

// Ÿ�̸ӿ� ���� ȣ��Ǵ� ��Ʈ��ũ�� ����� �Լ��Դϴ�.
void UUIHUDWidget::clearHitMark()
{
	switch (mCurrentMark)
	{
	case EHitType::Hit:
		mHitMark->SetVisibility(ESlateVisibility::Hidden);
		break;
	case EHitType::HeadShot:
		mHeadshotMark->SetVisibility(ESlateVisibility::Hidden);
		break;
	case EHitType::Kill:
		mKillMark->SetVisibility(ESlateVisibility::Hidden);
		break;
	}
	GetWorld()->GetTimerManager().ClearTimer(mHitMarkTimer);
}
