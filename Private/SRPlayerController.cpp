// Fill out your copyright notice in the Description page of Project Settings.
#include "SRPlayerController.h"
#include "SRGameMode.h"
#include "SRInGameSetting.h"
#include "SRPlayerCharacter.h"
#include "SRPlayerState.h"
#include "SRTargetManager.h"
#include "UIPauseWidget.h"
#include "UIHUDWidget.h"
#include "UIResultWidget.h"
#include "UISelectModesWidget.h"

#include "Kismet/KismetMathLibrary.h"

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

	static ConstructorHelpers::FClassFinder<UUserWidget> UI_IndicatorLine1440(TEXT("/Game/UI/UI_TargetIndicator_1440.UI_TargetIndicator_1440_C"));
	if (UI_IndicatorLine1440.Succeeded())
	{
		mTargetIndicateLine1440Class = UI_IndicatorLine1440.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> UI_IndicatorLine1080(TEXT("/Game/UI/UI_TargetIndicator_1080.UI_TargetIndicator_1080_C"));
	if (UI_IndicatorLine1080.Succeeded())
	{
		mTargetIndicateLine1080Class = UI_IndicatorLine1080.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> UI_IndicatorLine900(TEXT("/Game/UI/UI_TargetIndicator_900.UI_TargetIndicator_900_C"));
	if (UI_IndicatorLine900.Succeeded())
	{
		mTargetIndicateLine900Class = UI_IndicatorLine900.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> UI_IndicatorLine720(TEXT("/Game/UI/UI_TargetIndicator_720.UI_TargetIndicator_720_C"));
	if (UI_IndicatorLine720.Succeeded())
	{
		mTargetIndicateLine720Class = UI_IndicatorLine720.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> UI_IndicatorLine480(TEXT("/Game/UI/UI_TargetIndicator_480.UI_TargetIndicator_480_C"));
	if (UI_IndicatorLine480.Succeeded())
	{
		mTargetIndicateLine480Class = UI_IndicatorLine480.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> UI_IndicatorLineOtherSize(TEXT("/Game/UI/UI_TargetIndicator_OtherSize.UI_TargetIndicator_OtherSize_C"));
	if (UI_IndicatorLineOtherSize.Succeeded())
	{
		mTargetIndicateLineOtherSizeClass = UI_IndicatorLineOtherSize.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> UI_Indicator(TEXT("/Game/UI/UI_KillMark.UI_KillMark_C"));
	if (UI_Indicator.Succeeded())
	{
		mTargetIndicatorClass = UI_Indicator.Class;
	}

	mSpawnedTargetList.Reserve(4);
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

	initTargetIndicator();

	mHUDWidget->InitializeWidgets();
	mHUDWidget->AddToViewport();
	mHUDWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ASRPlayerController::UpdateTargetPositionFrom(ASRTargetManager& targetManager)
{
	mSpawnedTargetList.Empty();

	mSpawnedTargetList = targetManager.GetSpawnedTargetPositions();
}

void ASRPlayerController::CalcTargetIndicatorAndShow()
{
	auto* const gameMode = Cast<ASRGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	auto* const playerCharacter = Cast<ASRPlayerCharacter>(GetCharacter());
	ASRTargetManager* const targetManager = gameMode->GetTargetManager();

	// 이동형 표적이면 매 위젯 위치 계산시 타겟의 위치를 업데이트합니다.
	if(targetManager->IsMovableTargetMode())
	{
		UpdateTargetPositionFrom(*targetManager);
	}

	if(mSpawnedTargetList.Num() <= 0)
	{
		return;
	}

	FVector rightVector = playerCharacter->GetActorRightVector();
	const FVector characterPosition = playerCharacter->GetActorLocation();

	rightVector.Z = 0;
	rightVector.Normalize();

	for(size_t i = 0; i < mSpawnedTargetList.Num(); ++i)
	{
		// 타겟과 캐릭터의 벡터를 구합니다. 
		FVector characterToTarget = mSpawnedTargetList[i] - characterPosition;
		characterToTarget.Z = 0;
		characterToTarget.Normalize();

		// 내적을 이용하여 cos 값을 구합니다.
		float angle = FVector::DotProduct(characterToTarget, rightVector)/(characterToTarget.Size2D()*rightVector.Size2D());
		// cos 값을 통해 각도를 구합니다.
		float arcCos = FMath::RadiansToDegrees(FMath::Acos(angle));

		mTargetIndicatorWidget[i]->SetPositionInViewport(FVector2D(mViewportX - (arcCos * mIndicatorScale), mViewportY));
		mTargetIndicatorWidget[i]->SetVisibility(ESlateVisibility::Visible);
	}
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

void ASRPlayerController::SetAimingTypeToCharacter(const eAimingType newSetting)
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

void ASRPlayerController::InitCharacterMouseAndAimingSetting(const eScopeType scopeType) const
{
	ASRPlayerCharacter* const playerCharacter = Cast<ASRPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	switch(scopeType)
	{
		case eScopeType::Scope1X:
			playerCharacter->SetMouseSensitivity(mMouseSetting.HipX, mMouseSetting.HipY, mMouseSetting.Scope1X_X, mMouseSetting.Scope1X_Y);
			break;
		case eScopeType::Scope2dot5X:
			playerCharacter->SetMouseSensitivity(mMouseSetting.HipX, mMouseSetting.HipY, mMouseSetting.Scope2dot5X_X, mMouseSetting.Scope2dot5X_Y);
			break;
		case eScopeType::Scope6X:
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
		mAimingType = eAimingType::Hold;
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

	UE_LOG(LogTemp, Warning, TEXT("mSpawnedTargetList capacity = %d"), mSpawnedTargetList.GetAllocatedSize());


	ChangeInputMode(false);
}

void ASRPlayerController::initTargetIndicator()
{
	/*
	 * 타겟 표시기 수평선 위젯에 표시될 위젯을 생성합니다.
	 */
	mTargetIndicatorWidget[0] = CreateWidget<UUserWidget>(this, mTargetIndicatorClass);
	mTargetIndicatorWidget[1] = CreateWidget<UUserWidget>(this, mTargetIndicatorClass);

	mTargetIndicatorWidget[0]->AddToViewport();
	mTargetIndicatorWidget[1]->AddToViewport();

	mTargetIndicatorWidget[0]->SetVisibility(ESlateVisibility::Hidden);
	mTargetIndicatorWidget[1]->SetVisibility(ESlateVisibility::Hidden);

	/*
	 * 화면 크기에 따라서 위젯이 표시될 기준점과 수평선 위젯을 결정합니다.
	 */
	int32 sizeX = 0;
	int32 sizeY = 0;
	GetViewportSize(sizeX, sizeY);
	UE_LOG(LogTemp, Warning, TEXT("[initTargetIndicator] viewport size x = %d, size y = %d"), sizeX, sizeY);

	mViewportY = sizeY * 0.92f;

	switch (sizeY)
	{
	case 1440:
		// 테스트할 수 없음. 
		mViewportX = sizeX * 0.56f;
		mIndicatorScale = 2.2f;
		mTargetIndicateLineWidget = CreateWidget<UUserWidget>(this, mTargetIndicateLine1440Class);
		break;
	case 1080:
		mViewportX = sizeX * 0.63f;		// 180|------90|-------0|  0도의 기준점을 정합니다.
		mIndicatorScale = 2.5f;			// 뷰포트 크기마다 수직선 길이가 다르므로(블루프린트) 스케일을 지정합니다.
		mTargetIndicateLineWidget = CreateWidget<UUserWidget>(this, mTargetIndicateLine1080Class);
		break;
	case 900:
		mViewportX = sizeX * 0.65f;
		mIndicatorScale = 1.8f;
		mTargetIndicateLineWidget = CreateWidget<UUserWidget>(this, mTargetIndicateLine900Class);
		break;
	case 720:
		mViewportX = sizeX * 0.595f;
		mIndicatorScale = 1.5f;
		mTargetIndicateLineWidget = CreateWidget<UUserWidget>(this, mTargetIndicateLine720Class);
		break;
	case 480:
		mViewportX = sizeX * 0.58f;
		mIndicatorScale = 0.8f;
		mTargetIndicateLineWidget = CreateWidget<UUserWidget>(this, mTargetIndicateLine480Class);
		break;
	default: // in editor
		mViewportX = sizeX * 0.615f;
		mIndicatorScale = 1.13f;
		mTargetIndicateLineWidget = CreateWidget<UUserWidget>(this, mTargetIndicateLineOtherSizeClass);
		break;
	}
	mTargetIndicateLineWidget->AddToViewport();
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

