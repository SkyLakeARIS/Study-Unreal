// Fill out your copyright notice in the Description page of Project Settings.
#include "SRPlayerController.h"
#include "SRGameMode.h"
#include "SRInGameSetting.h"
#include "SRPlayerCharacter.h"
#include "SRPlayerState.h"
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
		mHUDClass = UI_INGAMEHUD.Class;
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

}

void ASRPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ASRPlayerController::ChangeInputMode(const bool bIsGameMode)
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


void ASRPlayerController::CreateUIWidgets()
{
	mSelectModesWidget = CreateWidget<UUISelectModesWidget>(this, mSelectModesWidgetClass);

	mHUDWidget = CreateWidget<UUIHUDWidget>(this, mHUDClass);

	mResultWidget = CreateWidget<UUIResultWidget>(this, mReusltWidgetClass);

	mPauseWidget = CreateWidget<UUIPauseWidget>(this, mPauseWidgetClass);

	mHUDWidget->InitializeWidgets();
	mHUDWidget->AddToViewport();
	mHUDWidget->SetVisibility(ESlateVisibility::Hidden);
}


ASRPlayerState* ASRPlayerController::GetPlayerState() const
{
	return Cast<ASRPlayerState>(PlayerState);
}

UUIHUDWidget* ASRPlayerController::GetHUDWidget() const
{
	return mHUDWidget;
}

UUISelectModesWidget* ASRPlayerController::GetSelectWidget() const
{
	return mSelectModesWidget;
}

FMouseSensitivity ASRPlayerController::GetMouseSensitivity() const
{
	return mMouseSetting;
}

void ASRPlayerController::SetMouseSensitivityAndUpdateToCharacter(const FMouseSensitivity newSetting)
{
	const ASRGameMode* const gameMode = Cast<ASRGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	mMouseSetting = newSetting;

	InitCharacterMouseAndAimingSetting(gameMode->GetScopeType());

	SaveMouseSensitivitySetting();
}

void ASRPlayerController::SetAimingTypeToCharacter(const EAimingType newSetting)
{
	ASRPlayerCharacter* const playerCharacter = Cast<ASRPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	mAimingType = newSetting;

	playerCharacter->SetAimingType(mAimingType);

	SaveMouseSensitivitySetting();
}

void ASRPlayerController::StartGame()
{
	ChangeInputMode(true);
	mHUDWidget->SetVisibility(ESlateVisibility::Visible);
}

void ASRPlayerController::EndGame()
{
	mHUDWidget->SetVisibility(ESlateVisibility::Hidden);
	mResultWidget->AddToViewport(3);
	mResultWidget->UpdateStageInfo(Cast<ASRPlayerState>(PlayerState));
	ChangeInputMode(false);
	SetPause(true);
}

void ASRPlayerController::InitCharacterMouseAndAimingSetting(const EScopeType scopeType) const
{
	ASRPlayerCharacter* const playerCharacter = Cast<ASRPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	switch(scopeType)
	{
		case EScopeType::Scope1X:
			playerCharacter->SetMouseSensitivity(mMouseSetting.HipX, mMouseSetting.HipY, mMouseSetting.Scope1X_X, mMouseSetting.Scope1X_Y);
			break;
		case EScopeType::Scope2dot5X:
			playerCharacter->SetMouseSensitivity(mMouseSetting.HipX, mMouseSetting.HipY, mMouseSetting.Scope2dot5X_X, mMouseSetting.Scope2dot5X_Y);
			break;
		case EScopeType::Scope6X:
			playerCharacter->SetMouseSensitivity(mMouseSetting.HipX, mMouseSetting.HipY, mMouseSetting.Scope6X_X, mMouseSetting.Scope6X_Y);
			break;
		default:
			checkf(false, TEXT("올바르지 않은 EScopeType입니다."));
	}

	playerCharacter->SetAimingType(mAimingType);
}

void ASRPlayerController::LoadMouseSensitivitySetting()
{
	const auto* settingData = Cast<USRInGameSetting>(UGameplayStatics::CreateSaveGameObject(USRInGameSetting::StaticClass()));

	settingData = Cast<USRInGameSetting>(UGameplayStatics::LoadGameFromSlot(settingData->GetSlotName(), settingData->GetSlotIndex()));
	if (settingData)
	{
		mMouseSetting = settingData->MouseSensitivity;
		mAimingType = settingData->AimingType;
	}
	else // 기존에 저장된 데이터가 없거나 불러올 수 없으면 기본값으로 초기화합니다.
	{
		const FMouseSensitivity initMouseSetting;
		mMouseSetting = initMouseSetting;
		mAimingType = EAimingType::Hold;
	}
}

void ASRPlayerController::SaveMouseSensitivitySetting() const
{
	auto* const settingData = Cast<USRInGameSetting>(UGameplayStatics::CreateSaveGameObject(USRInGameSetting::StaticClass()));
	settingData->MouseSensitivity = mMouseSetting;
	settingData->AimingType = mAimingType;
	UGameplayStatics::SaveGameToSlot(settingData, settingData->GetSlotName(), settingData->GetSlotIndex());

}

void ASRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction(TEXT("Pause"), EInputEvent::IE_Pressed, this, &ASRPlayerController::PauseGame);
}

void ASRPlayerController::BeginPlay()
{
	Super::BeginPlay();


	ChangeInputMode(false);
}

void ASRPlayerController::PauseGame()
{
	mPauseWidget = CreateWidget<UUIPauseWidget>(this, mPauseWidgetClass);

	auto* gameMode = Cast<ASRGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	gameMode->PauseGame();

	mPauseWidget->AddToViewport(3);
	mPauseWidget->UpdateInfoWhenOpen();

	SetPause(true);
	ChangeInputMode(false);
	mHUDWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ASRPlayerController::ResumeGame()
{
	ChangeInputMode(true);
	SetPause(false);

	mHUDWidget->SetVisibility(ESlateVisibility::Visible);

	auto* gameMode = Cast<ASRGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	gameMode->ResumeGame();
}

