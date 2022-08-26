// Fill out your copyright notice in the Description page of Project Settings.
#include "UISelectWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"


void USelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BtnShortRange = Cast<UButton>(GetWidgetFromName(TEXT("Btn_ShortRange")));
	BtnMidRange = Cast<UButton>(GetWidgetFromName(TEXT("Btn_MidRange")));
	BtnLongRange = Cast<UButton>(GetWidgetFromName(TEXT("Btn_LongRange")));

	BtnMovableShortRange = Cast<UButton>(GetWidgetFromName(TEXT("Btn_MovableShortRange")));
	BtnMovableMidRange = Cast<UButton>(GetWidgetFromName(TEXT("Btn_MovableMidRange")));
	BtnMovableLongRange = Cast<UButton>(GetWidgetFromName(TEXT("Btn_MovableLongRange")));

	BtnShortRange->OnClicked.AddDynamic(this, &USelectWidget::OnShortRangeClicked);
	BtnMidRange->OnClicked.AddDynamic(this, &USelectWidget::OnMidRangeClicked);
	BtnLongRange->OnClicked.AddDynamic(this, &USelectWidget::OnLongRangeClicked);

	BtnMovableShortRange->OnClicked.AddDynamic(this, &USelectWidget::OnMovableShortRangeClicked);
	BtnMovableMidRange->OnClicked.AddDynamic(this, &USelectWidget::OnMovableMidRangeClicked);
	BtnMovableLongRange->OnClicked.AddDynamic(this, &USelectWidget::OnMovableLongRangeClicked);
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
