// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterPlayerController.h"

#include "UIPauseWidget.h"
#include "UIHUDWidget.h"
#include "UIResultWidget.h"

ACharacterPlayerController::ACharacterPlayerController()
{
	static ConstructorHelpers::FClassFinder<UUIPauseWidget> UI_INGAMEMENU(TEXT("/Game/UI/UI_PauseMenu.UI_PauseMenu_C"));
	if (UI_INGAMEMENU.Succeeded())
	{
		mPauseWidgetClass = UI_INGAMEMENU.Class;
	}
	static ConstructorHelpers::FClassFinder<UUIHUDWidget> UI_INGAMEHUD(TEXT("/Game/UI/UI_HUD.UI_HUD_C"));
	if (UI_INGAMEHUD.Succeeded())
	{
		HUDClass = UI_INGAMEHUD.Class;
	}
	static ConstructorHelpers::FClassFinder<UUIResultWidget> UI_RESULT(TEXT("/Game/UI/UI_Result.UI_Result_C"));
	if (UI_RESULT.Succeeded())
	{
		mReusltWidgetClass = UI_RESULT.Class;
	}
	remainingTime = 60;
}

void ACharacterPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void ACharacterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ACharacterPlayerController::ChangeInputMode(bool bIsGameMode)
{
	if (bIsGameMode)
	{
		SetInputMode(GameInputMode);
		bShowMouseCursor = false;
	}
	else
	{
		SetInputMode(UIInputMode);
		bShowMouseCursor = true;
	}
}

void ACharacterPlayerController::ResumeCountDown()
{
	GetWorld()->GetTimerManager().UnPauseTimer(THCountDown);
}

void ACharacterPlayerController::ClearCountDown()
{
	GetWorld()->GetTimerManager().ClearTimer(THCountDown);
}

void ACharacterPlayerController::BindStatToUI()
{
	mPlayerState->BindHUD(InGameHUD);
}

ASRPlayerState* ACharacterPlayerController::GetPlayerState() const
{
	return mPlayerState;
}

UUIHUDWidget* ACharacterPlayerController::GetIngameHUD() const
{
	return InGameHUD;
}

void ACharacterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
		InputComponent->BindAction(TEXT("Pause"), EInputEvent::IE_Pressed, this, &ACharacterPlayerController::PauseGame);

}

void ACharacterPlayerController::BeginPlay()
{
	Super::BeginPlay();
		ChangeInputMode(true);

	if(IsLocalController() && HUDClass)
	{
		InGameHUD = CreateWidget<UUIHUDWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), HUDClass);
		InGameHUD->AddToViewport();
	}

	mPlayerState = Cast<ASRPlayerState>(PlayerState);
	
	GetWorld()->GetTimerManager().SetTimer(THCountDown, this, &ACharacterPlayerController::countDown, 1.0f, true, 0.0f);
	BindStatToUI();

}

void ACharacterPlayerController::PauseGame()
{
	UE_LOG(LogTemp, Warning, TEXT("GamePuaue"));
	InGameHUD->SetVisibility(ESlateVisibility::Hidden);
	mPauseWidget = CreateWidget<UUIPauseWidget>(this, mPauseWidgetClass);
	mPauseWidget->AddToViewport(3);
	SetPause(true);
	ChangeInputMode(false);
}


void ACharacterPlayerController::countDown()
{
	--remainingTime;
	InGameHUD->UpdateRemainingTime(remainingTime);
	if(remainingTime <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(THCountDown);
		result();
	}
}

void ACharacterPlayerController::result()
{
	InGameHUD->SetVisibility(ESlateVisibility::Hidden);
	mResultWidget = CreateWidget<UUIResultWidget>(this, mReusltWidgetClass);
	mResultWidget->AddToViewport(3);
	mResultWidget->UpdateStageInfo(mPlayerState);
	ChangeInputMode(false);
	SetPause(true);
}
