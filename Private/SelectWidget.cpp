// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"


void USelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BtnDev = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Dev")));
	BtnCombination = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Combination")));

	BtnDev->OnClicked.AddDynamic(this, &USelectWidget::OnDevClicked);
	BtnCombination->OnClicked.AddDynamic(this, &USelectWidget::OnCombinationClicked);

}

void USelectWidget::OnDevClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Dev1"));
	
}

void USelectWidget::OnCombinationClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Map1"));
}
