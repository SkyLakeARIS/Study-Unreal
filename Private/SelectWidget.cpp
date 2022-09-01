// Fill out your copyright notice in the Description page of Project Settings.
#include "UISelectWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"


void USelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	mBtnShortRange = Cast<UButton>(GetWidgetFromName(TEXT("Btn_ShortRange")));
	mBtnMidRange = Cast<UButton>(GetWidgetFromName(TEXT("Btn_MidRange")));
	mBtnLongRange = Cast<UButton>(GetWidgetFromName(TEXT("Btn_LongRange")));

	mBtnMovableShortRange = Cast<UButton>(GetWidgetFromName(TEXT("Btn_MovableShortRange")));
	mBtnMovableMidRange = Cast<UButton>(GetWidgetFromName(TEXT("Btn_MovableMidRange")));
	mBtnMovableLongRange = Cast<UButton>(GetWidgetFromName(TEXT("Btn_MovableLongRange")));

	mBtnShortRange->OnClicked.AddDynamic(this, &USelectWidget::OnShortRangeClicked);
	mBtnMidRange->OnClicked.AddDynamic(this, &USelectWidget::OnMidRangeClicked);
	mBtnLongRange->OnClicked.AddDynamic(this, &USelectWidget::OnLongRangeClicked);

	mBtnMovableShortRange->OnClicked.AddDynamic(this, &USelectWidget::OnMovableShortRangeClicked);
	mBtnMovableMidRange->OnClicked.AddDynamic(this, &USelectWidget::OnMovableMidRangeClicked);
	mBtnMovableLongRange->OnClicked.AddDynamic(this, &USelectWidget::OnMovableLongRangeClicked);
}

void USelectWidget::OnShortRangeClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("ShortRange"));
}

void USelectWidget::OnMidRangeClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("MidRange"));
}

void USelectWidget::OnLongRangeClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("LongRange"));
}

void USelectWidget::OnMovableShortRangeClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("MovableShortRange"));
}

void USelectWidget::OnMovableMidRangeClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("MovableMidRange"));
}

void USelectWidget::OnMovableLongRangeClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("MovableLongRange"));
}
