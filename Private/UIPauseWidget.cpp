// Fill out your copyright notice in the Description page of Project Settings.


#include "UIPauseWidget.h"
#include "CharacterPlayerController.h"
#include "UIHUDWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UUIPauseWidget::OnResumeClicked()
{
	auto playerController = Cast<ACharacterPlayerController>(GetOwningPlayer());

	RemoveFromParent();
	playerController->ChangeInputMode(true);
	playerController->SetPause(false);
	playerController->GetIngameHUD()->SetVisibility(ESlateVisibility::Visible);
	playerController->ResumeCountDown();

}

void UUIPauseWidget::OnRetryClicked()
{
	auto playerController = Cast<ACharacterPlayerController>(GetOwningPlayer());
	playerController->ClearCountDown();
	playerController->RestartLevel();
}

void UUIPauseWidget::OnReturnMenuClicked()
{
	auto playerController = Cast<ACharacterPlayerController>(GetOwningPlayer());
	playerController->ClearCountDown();
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMenu"));
}

void UUIPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UE_LOG(LogTemp, Warning, TEXT("Ingame UI - Nativecons"));

	btnResume = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Resume")));
	btnRetry = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Retry")));
	btnReturnMenu = Cast<UButton>(GetWidgetFromName(TEXT("Btn_ReturnMenu")));

	btnResume->OnClicked.AddDynamic(this, &UUIPauseWidget::OnResumeClicked);
	btnRetry->OnClicked.AddDynamic(this, &UUIPauseWidget::OnRetryClicked);
	btnReturnMenu->OnClicked.AddDynamic(this, &UUIPauseWidget::OnReturnMenuClicked);
}
