// Fill out your copyright notice in the Description page of Project Settings.
#include "SRPlayerController.h"
#include "SRPlayerState.h"
#include "SRSpawnPoint.h"
#include "SRTargetManager.h"
#include "UIPauseWidget.h"
#include "UIHUDWidget.h"
#include "UIResultWidget.h"
#include "UISelectModesWidget.h"

ASRPlayerController::ASRPlayerController()
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
	static ConstructorHelpers::FClassFinder<UUISelectModesWidget> UI_SELECTMODES(TEXT("/Game/UI/UI_SelectModes.UI_SelectModes_C"));
	if (UI_SELECTMODES.Succeeded())
	{
		mSelectModesWidgetClass = UI_SELECTMODES.Class;
	}
	remainingTime = 90;
	mTimeToReady = 5;
	mSensitivity = 45.0f;
	mbDebugMode = false;
	mbStartGame = false;
}

void ASRPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASRPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ASRPlayerController::ChangeInputMode(bool bIsGameMode)
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

void ASRPlayerController::ResumeCountDown() const
{
	GetWorld()->GetTimerManager().UnPauseTimer(THCountDown);
}

void ASRPlayerController::ClearCountDown()
{
	GetWorld()->GetTimerManager().ClearTimer(THCountDown);
}

void ASRPlayerController::BindStatToUI()
{
	mPlayerState->BindHUD(InGameHUD);
}

void ASRPlayerController::ShowInGameHUDAndStartTimer()
{
	ChangeInputMode(true);
	InGameHUD->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().SetTimer(THCountDown, this, &ASRPlayerController::countReadyTime, 1.0f, true, 0.0f);
}

void ASRPlayerController::SetDebugMode(bool active)
{
	mbDebugMode = active;
}

bool ASRPlayerController::IsDebugging() const
{
	return mbDebugMode;
}

// 게임을 시작 했는지 여부를 반환합니다. (false - 준비 시간, true - 게임 시작)
bool ASRPlayerController::IsStartMainGame() const
{
	return mbStartGame;
}

ASRPlayerState* ASRPlayerController::GetPlayerState() const
{
	return mPlayerState;
}

UUIHUDWidget* ASRPlayerController::GetIngameHUD() const
{
	return InGameHUD;
}

UUISelectModesWidget* ASRPlayerController::GetSelectModesWidget() const
{
	return mSelectModesWidget;
}

ASRTargetManager* ASRPlayerController::GetTargetManager() const
{
	return mTargetManager;
}

void ASRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction(TEXT("Pause"), EInputEvent::IE_Pressed, this, &ASRPlayerController::PauseGame);
}

void ASRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(IsLocalController() && HUDClass)
	{
		InGameHUD = CreateWidget<UUIHUDWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), HUDClass);
	}

	if (IsLocalController() && mSelectModesWidgetClass)
	{
		mSelectModesWidget = CreateWidget<UUISelectModesWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), mSelectModesWidgetClass);
	}


	mPlayerState = Cast<ASRPlayerState>(PlayerState);

	ChangeInputMode(false);
	mSelectModesWidget->AddToViewport();

	InGameHUD->InitializeWidgets();
	InGameHUD->AddToViewport();
	InGameHUD->SetVisibility(ESlateVisibility::Hidden);

	BindStatToUI();
	mTargetManager = GetWorld()->SpawnActor<ASRTargetManager>(ASRTargetManager::StaticClass(), FVector::ZeroVector,  FRotator::ZeroRotator);
}

void ASRPlayerController::PauseGame()
{
	InGameHUD->SetVisibility(ESlateVisibility::Hidden);
	mPauseWidget = CreateWidget<UUIPauseWidget>(this, mPauseWidgetClass);
	mPauseWidget->AddToViewport(3);
	mPauseWidget->UpdateInfoWhenOpen();
	SetPause(true);
	ChangeInputMode(false);
}


void ASRPlayerController::countDownMainTime()
{
	--remainingTime;
	InGameHUD->UpdateRemainingTime(remainingTime);
	if(remainingTime <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(THCountDown);
		result();
	}
}

void ASRPlayerController::countReadyTime()
{
	--mTimeToReady;
	InGameHUD->UpdateRemainingTime(mTimeToReady);
	if (mTimeToReady <= 0)
	{
		mTargetManager->RandomTargetSpawn();
		mTargetManager->RandomTargetSpawn();
		mbStartGame = true;
		GetWorld()->GetTimerManager().ClearTimer(THCountDown);
		GetWorld()->GetTimerManager().SetTimer(THCountDown, this, &ASRPlayerController::countDownMainTime, 1.0f, true, 0.0f);
	}
}

void ASRPlayerController::result()
{
	InGameHUD->SetVisibility(ESlateVisibility::Hidden);
	mResultWidget = CreateWidget<UUIResultWidget>(this, mReusltWidgetClass);
	mResultWidget->AddToViewport(3);
	mResultWidget->UpdateStageInfo(mPlayerState);
	ChangeInputMode(false);
	SetPause(true);
}
