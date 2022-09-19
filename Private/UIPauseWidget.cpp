// Fill out your copyright notice in the Description page of Project Settings.
#include "UIPauseWidget.h"
#include <string>
#include "SRPlayerController.h"
#include "UIHUDWidget.h"
#include "SRPlayerCharacter.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"


void UUIPauseWidget::OnResumeClicked()
{
	auto* playerController = Cast<ASRPlayerController>(GetOwningPlayer());

	RemoveFromParent();
	playerController->ChangeInputMode(true);
	playerController->SetPause(false);
	playerController->GetIngameHUD()->SetVisibility(ESlateVisibility::Visible);
	playerController->ResumeCountDown();
}

void UUIPauseWidget::OnRetryClicked()
{
	auto* playerController = Cast<ASRPlayerController>(GetOwningPlayer());
	playerController->ClearCountDown();
	playerController->RestartLevel();
}

void UUIPauseWidget::OnReturnMenuClicked()
{
	auto* playerController = Cast<ASRPlayerController>(GetOwningPlayer());
	playerController->ClearCountDown();
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMenu"));
}

void UUIPauseWidget::onChangeHip_X(const FText& InText, ETextCommit::Type InCommitType)
{
	auto* player = Cast<ASRPlayerCharacter>(GetOwningPlayerPawn());

	int32 newValue = 0;
	if (!StringToInteger(InText, &newValue)) 
	{
		mHipX->SetText(FText::FromString(FString::FromInt(player->MouseSetting.HipX)));
		return;
	}

	player->MouseSetting.HipX = newValue;
	player->SaveInGameSetting();

	mHipX->SetText(FText::FromString(FString::FromInt(player->MouseSetting.HipX)));
	mMessageBox->SetText(FText::FromString(TEXT("설정이 저장되었습니다.")));
}

void UUIPauseWidget::onChangeHip_Y(const FText& InText, ETextCommit::Type InCommitType)
{
	auto* player = Cast<ASRPlayerCharacter>(GetOwningPlayerPawn());
	int32 newValue = 0;
	if (!StringToInteger(InText, &newValue)) 
	{
		mHipY->SetText(FText::FromString(FString::FromInt(player->MouseSetting.HipY)));
		return;
	}

	player->MouseSetting.HipY = newValue;
	player->SaveInGameSetting();

	mHipY->SetText(FText::FromString(FString::FromInt(player->MouseSetting.HipY)));
	mMessageBox->SetText(FText::FromString(TEXT("설정이 저장되었습니다.")));
}

void UUIPauseWidget::onChangeScope1X_X(const FText& InText, ETextCommit::Type InCommitType)
{
	auto* player = Cast<ASRPlayerCharacter>(GetOwningPlayerPawn());
	int32 newValue = 0;
	if (!StringToInteger(InText, &newValue)) 
	{
		mScope1X_X->SetText(FText::FromString(FString::FromInt(player->MouseSetting.Scope1X_X)));
		return;
	}

	player->MouseSetting.Scope1X_X = newValue;
	player->SaveInGameSetting();

	mScope1X_X->SetText(FText::FromString(FString::FromInt(player->MouseSetting.Scope1X_X)));
	mMessageBox->SetText(FText::FromString(TEXT("설정이 저장되었습니다.")));
}

void UUIPauseWidget::onChangeScope1X_Y(const FText& InText, ETextCommit::Type InCommitType)
{
	auto* player = Cast<ASRPlayerCharacter>(GetOwningPlayerPawn());
	int32 newValue = 0;
	if (!StringToInteger(InText, &newValue))
	{
		mScope1X_Y->SetText(FText::FromString(FString::FromInt(player->MouseSetting.Scope1X_Y)));
		return;
	}

	player->MouseSetting.Scope1X_Y = newValue;
	player->SaveInGameSetting();

	mScope1X_Y->SetText(FText::FromString(FString::FromInt(player->MouseSetting.Scope1X_Y)));
	mMessageBox->SetText(FText::FromString(TEXT("설정이 저장되었습니다.")));
}

void UUIPauseWidget::onChangeScope2dot5X_X(const FText& InText, ETextCommit::Type InCommitType)
{
	auto* player = Cast<ASRPlayerCharacter>(GetOwningPlayerPawn());
	int32 newValue = 0;
	if (!StringToInteger(InText, &newValue)) 
	{
		mScope2dot5X_X->SetText(FText::FromString(FString::FromInt(player->MouseSetting.Scope2dot5X_X)));
		return;
	}

	player->MouseSetting.Scope2dot5X_X = newValue;
	player->SaveInGameSetting();

	mScope2dot5X_X->SetText(FText::FromString(FString::FromInt(player->MouseSetting.Scope2dot5X_X)));
	mMessageBox->SetText(FText::FromString(TEXT("설정이 저장되었습니다.")));
}

void UUIPauseWidget::onChangeScope2dot5X_Y(const FText& InText, ETextCommit::Type InCommitType)
{
	auto* player = Cast<ASRPlayerCharacter>(GetOwningPlayerPawn());
	int32 newValue = 0;
	if (!StringToInteger(InText, &newValue)) 
	{
		mScope2dot5X_Y->SetText(FText::FromString(FString::FromInt(player->MouseSetting.Scope2dot5X_Y)));
		return;
	}

	player->MouseSetting.Scope2dot5X_Y = newValue;
	player->SaveInGameSetting();

	mScope2dot5X_Y->SetText(FText::FromString(FString::FromInt(player->MouseSetting.Scope2dot5X_Y)));
	mMessageBox->SetText(FText::FromString(TEXT("설정이 저장되었습니다.")));
}

void UUIPauseWidget::onChangeScope6X_X(const FText& InText, ETextCommit::Type InCommitType)
{
	auto* player = Cast<ASRPlayerCharacter>(GetOwningPlayerPawn());
	int32 newValue = 0;
	if (!StringToInteger(InText, &newValue))
	{
		mScope6X_X->SetText(FText::FromString(FString::FromInt(player->MouseSetting.Scope6X_X)));
		return;
	}

	player->MouseSetting.Scope6X_X = newValue;
	player->SaveInGameSetting();

	mScope6X_X->SetText(FText::FromString(FString::FromInt(player->MouseSetting.Scope6X_X)));
	mMessageBox->SetText(FText::FromString(TEXT("설정이 저장되었습니다.")));
}

void UUIPauseWidget::onChangeScope6X_Y(const FText& InText, ETextCommit::Type InCommitType)
{
	auto* player = Cast<ASRPlayerCharacter>(GetOwningPlayerPawn());
	int32 newValue = 0;
	if (!StringToInteger(InText, &newValue))
	{
		mScope6X_Y->SetText(FText::FromString(FString::FromInt(player->MouseSetting.Scope6X_Y)));
		return;
	}
	
	player->MouseSetting.Scope6X_Y = newValue;
	player->SaveInGameSetting();

	mScope6X_Y->SetText(FText::FromString(FString::FromInt(player->MouseSetting.Scope6X_Y)));
	mMessageBox->SetText(FText::FromString(TEXT("설정이 저장되었습니다.")));
}

void UUIPauseWidget::onHoldClicked()
{
	auto* player = Cast<ASRPlayerCharacter>(GetOwningPlayerPawn());
	player->SetAimToggleOrHold(EAimingType::Hold);

	mBtnHold->SetIsEnabled(false);
	mBtnToggle->SetIsEnabled(true);

	player->SaveInGameSetting();
	mMessageBox->SetText(FText::FromString(TEXT("설정이 저장되었습니다.")));
}

void UUIPauseWidget::onToggleClicked()
{
	auto* player = Cast<ASRPlayerCharacter>(GetOwningPlayerPawn());
	player->SetAimToggleOrHold(EAimingType::Toggle);

	mBtnHold->SetIsEnabled(true);
	mBtnToggle->SetIsEnabled(false);

	player->SaveInGameSetting();
	mMessageBox->SetText(FText::FromString(TEXT("설정이 저장되었습니다.")));
}

void UUIPauseWidget::onDebugChecked(bool active)
{
	auto* playerController = Cast<ASRPlayerController>(GetOwningPlayer());
	playerController->SetDebugMode(active);
}

bool UUIPauseWidget::StringToInteger(const FText& InText, int32* int_Out)
{
	if (!InText.IsNumeric())
	{
		mMessageBox->SetText(FText::FromString(TEXT("정수 이외의 값은 적용되지 않습니다.")));
		return false;
	}

	int32 integer = std::stoi(*InText.ToString());

	integer = FMath::Clamp(integer, MIN_SETTING_VALUE, MAX_SETTING_VALUE);
	*int_Out = integer;

	return true;
}

void UUIPauseWidget::UpdateInfoWhenOpen()
{
	const auto* player = Cast<ASRPlayerCharacter>(GetOwningPlayerPawn());
	const auto* playerController = Cast<ASRPlayerController>(GetOwningPlayer());

	mHipX->SetText(FText::FromString(FString::FromInt(player->MouseSetting.HipX)));
	mHipY->SetText(FText::FromString(FString::FromInt(player->MouseSetting.HipY)));
	mScope1X_X->SetText(FText::FromString(FString::FromInt(player->MouseSetting.Scope1X_X)));
	mScope1X_Y->SetText(FText::FromString(FString::FromInt(player->MouseSetting.Scope1X_Y)));
	mScope2dot5X_X->SetText(FText::FromString(FString::FromInt(player->MouseSetting.Scope2dot5X_X)));
	mScope2dot5X_Y->SetText(FText::FromString(FString::FromInt(player->MouseSetting.Scope2dot5X_Y)));
	mScope6X_X->SetText(FText::FromString(FString::FromInt(player->MouseSetting.Scope6X_X)));
	mScope6X_Y->SetText(FText::FromString(FString::FromInt(player->MouseSetting.Scope6X_Y)));

	if (player->GetAimingType() == EAimingType::Toggle)
	{
		mBtnToggle->SetIsEnabled(false);
	}
	else
	{
		mBtnHold->SetIsEnabled(false);
	}

	mMessageBox->SetText(FText::FromString(TEXT("정수 값을 입력하여 감도를 변경할 수 있습니다.")));

	if(playerController->IsDebugging())
	{
		mDebugMode->SetCheckedState(ECheckBoxState::Checked);
	}
	else
	{
		mDebugMode->SetCheckedState(ECheckBoxState::Unchecked);
	}
}

void UUIPauseWidget::NativeConstruct()
{
	Super::NativeConstruct();

	mBtnResume = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Resume")));
	mBtnRetry = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Retry")));
	mBtnReturnMenu = Cast<UButton>(GetWidgetFromName(TEXT("Btn_ReturnMenu")));
	mBtnHold = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Hold")));
	mBtnToggle = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Toggle")));

	mHipX = Cast<UEditableText>(GetWidgetFromName(FName("Hip_X")));
	mHipY = Cast<UEditableText>(GetWidgetFromName(TEXT("Hip_Y")));
	mScope1X_X = Cast<UEditableText>(GetWidgetFromName(TEXT("Optic_X")));
	mScope1X_Y = Cast<UEditableText>(GetWidgetFromName(TEXT("Optic_Y")));
	mScope2dot5X_X = Cast<UEditableText>(GetWidgetFromName(TEXT("ACOG_X")));
	mScope2dot5X_Y = Cast<UEditableText>(GetWidgetFromName(TEXT("ACOG_Y")));
	mScope6X_X = Cast<UEditableText>(GetWidgetFromName(TEXT("Scope_X")));
	mScope6X_Y = Cast<UEditableText>(GetWidgetFromName(TEXT("Scope_Y")));

	mMessageBox = Cast<UTextBlock>(GetWidgetFromName(TEXT("MessageBox")));

	mDebugMode = Cast<UCheckBox>(GetWidgetFromName(TEXT("CB_DebugMode")));

	mBtnResume->OnClicked.AddDynamic(this, &UUIPauseWidget::OnResumeClicked);
	mBtnRetry->OnClicked.AddDynamic(this, &UUIPauseWidget::OnRetryClicked);
	mBtnReturnMenu->OnClicked.AddDynamic(this, &UUIPauseWidget::OnReturnMenuClicked);
	mBtnHold->OnClicked.AddDynamic(this, &UUIPauseWidget::onHoldClicked);
	mBtnToggle->OnClicked.AddDynamic(this, &UUIPauseWidget::onToggleClicked);

	mHipX->OnTextCommitted.AddDynamic(this, &UUIPauseWidget::onChangeHip_X);
	mHipY->OnTextCommitted.AddDynamic(this, &UUIPauseWidget::onChangeHip_Y);
	mScope1X_X->OnTextCommitted.AddDynamic(this, &UUIPauseWidget::onChangeScope1X_X);
	mScope1X_Y->OnTextCommitted.AddDynamic(this, &UUIPauseWidget::onChangeScope1X_Y);
	mScope2dot5X_X->OnTextCommitted.AddDynamic(this, &UUIPauseWidget::onChangeScope2dot5X_X);
	mScope2dot5X_Y->OnTextCommitted.AddDynamic(this, &UUIPauseWidget::onChangeScope2dot5X_Y);
	mScope6X_X->OnTextCommitted.AddDynamic(this, &UUIPauseWidget::onChangeScope6X_X);
	mScope6X_Y->OnTextCommitted.AddDynamic(this, &UUIPauseWidget::onChangeScope6X_Y);

	mDebugMode->OnCheckStateChanged.AddDynamic(this, &UUIPauseWidget::onDebugChecked);

	mMessageBox->SetText(FText::FromString(TEXT("정수 값을 입력하여 감도를 변경할 수 있습니다.")));

	mbCheckDebugMode = false;
}
