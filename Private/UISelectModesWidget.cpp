// Fill out your copyright notice in the Description page of Project Settings.


#include "UISelectModesWidget.h"
#include "CharacterPlayerController.h"
#include "VersionCharacter.h"
#include "SRTargetManager.h"
#include "Components/Button.h"
#include "Engine/AssetManager.h"

void UUISelectModesWidget::NativeConstruct()
{
	Super::NativeConstruct();
	mBtnAR = Cast<UButton>(GetWidgetFromName(TEXT("Btn_AR")));
	mBtnHG = Cast<UButton>(GetWidgetFromName(TEXT("Btn_HG")));
	mBtnSR = Cast<UButton>(GetWidgetFromName(TEXT("Btn_SR")));
	mBtn1X = Cast<UButton>(GetWidgetFromName(TEXT("Btn_1X")));
	mBtn2Dot5X = Cast<UButton>(GetWidgetFromName(TEXT("Btn_2Dot5X")));
	mBtn6X = Cast<UButton>(GetWidgetFromName(TEXT("Btn_6X")));
	mBtnBattlefield = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Battlefield")));
	mBtnRainbowSix = Cast<UButton>(GetWidgetFromName(TEXT("Btn_RainbowSix")));
	mBtnTarkov = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Tarkov")));
	mBtnStart = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Start")));
	mBtnPlateType = Cast<UButton>(GetWidgetFromName(TEXT("Btn_PlateType")));
	mBtnCharacterType = Cast<UButton>(GetWidgetFromName(TEXT("Btn_CharacterType")));

	mBtnAR->OnClicked.AddDynamic(this, &UUISelectModesWidget::clickedAR);
	mBtnHG->OnClicked.AddDynamic(this, &UUISelectModesWidget::clickedHG);
	mBtnSR->OnClicked.AddDynamic(this, &UUISelectModesWidget::clickedSR);
	mBtn1X->OnClicked.AddDynamic(this, &UUISelectModesWidget::clicked1X);
	mBtn2Dot5X->OnClicked.AddDynamic(this, &UUISelectModesWidget::clicked2Dot5X);
	mBtn6X->OnClicked.AddDynamic(this, &UUISelectModesWidget::clicked6X);
	mBtnBattlefield->OnClicked.AddDynamic(this, &UUISelectModesWidget::clickedBattlefield);
	mBtnRainbowSix->OnClicked.AddDynamic(this, &UUISelectModesWidget::clickedRainbowSix);
	mBtnTarkov->OnClicked.AddDynamic(this, &UUISelectModesWidget::clickedTarkov);
	mBtnStart->OnClicked.AddDynamic(this, &UUISelectModesWidget::clickedStart);
	mBtnPlateType->OnClicked.AddDynamic(this, &UUISelectModesWidget::clickedPlateType);
	mBtnCharacterType->OnClicked.AddDynamic(this, &UUISelectModesWidget::clickedCharacterType);

	mbIsSelectWeapon = false;
	mbIsSelectOptic = false;
	mbIsSelectMode = false;
	mbCanClickStart = false;
}

void UUISelectModesWidget::BindCharacterInfo(AVersionCharacter* character)
{
	mOnGameData.BindUObject(character, &AVersionCharacter::InitGameMode);
	UE_LOG(LogTemp, Warning, TEXT("UI-Char : Bind End"));
	mbCanClickStart = true;
}


void UUISelectModesWidget::clickedAR()
{
	mBtnAR->SetIsEnabled(false);
	mBtnHG->SetIsEnabled(true);
	mBtnSR->SetIsEnabled(true);
	mSelectedModes.weapon = EWeaponType::AR;
	mbIsSelectWeapon = true;

	// AR의 경우에는 모든 조준경을 장착할 수 있음.
	mBtn2Dot5X->SetIsEnabled(true);
	mBtn6X->SetIsEnabled(true);
}

void UUISelectModesWidget::clickedHG()
{
	mBtnAR->SetIsEnabled(true);
	mBtnHG->SetIsEnabled(false);
	mBtnSR->SetIsEnabled(true);
	mbIsSelectWeapon = true;

	// HG의 경우 1배율 조준경만 장착 가능하도록 제한
	mBtn1X->SetIsEnabled(false);
	mBtn2Dot5X->SetIsEnabled(true);
	mBtn6X->SetIsEnabled(true);

	mBtn2Dot5X->SetIsEnabled(false);
	mBtn6X->SetIsEnabled(false);
	mSelectedModes.scope = EScopeType::Scope1X;
	mbIsSelectOptic = true;

	mSelectedModes.weapon = EWeaponType::HG;
}

void UUISelectModesWidget::clickedSR()
{
	mBtnAR->SetIsEnabled(true);
	mBtnHG->SetIsEnabled(true);
	mBtnSR->SetIsEnabled(false);
	mSelectedModes.weapon = EWeaponType::SR;
	mbIsSelectWeapon = true;

	// SR의 경우에는 모든 조준경을 장착할 수 있음.
	mBtn2Dot5X->SetIsEnabled(true);
	mBtn6X->SetIsEnabled(true);
}

void UUISelectModesWidget::clicked1X()
{
	mBtn1X->SetIsEnabled(false);
	mBtn2Dot5X->SetIsEnabled(true);
	mBtn6X->SetIsEnabled(true);
	mSelectedModes.scope = EScopeType::Scope1X;
	mbIsSelectOptic = true;
}

void UUISelectModesWidget::clicked2Dot5X()
{
	mBtn1X->SetIsEnabled(true);
	mBtn2Dot5X->SetIsEnabled(false);
	mBtn6X->SetIsEnabled(true);
	mSelectedModes.scope = EScopeType::Scope2dot5X;
	mbIsSelectOptic = true;

}

void UUISelectModesWidget::clicked6X()
{
	mBtn1X->SetIsEnabled(true);
	mBtn2Dot5X->SetIsEnabled(true);
	mBtn6X->SetIsEnabled(false);
	mSelectedModes.scope = EScopeType::Scope6X;
	mbIsSelectOptic = true;

}

void UUISelectModesWidget::clickedBattlefield()
{
	mBtnBattlefield->SetIsEnabled(false);
	mBtnRainbowSix->SetIsEnabled(true);
	mBtnTarkov->SetIsEnabled(true);
	mSelectedModes.game = EGameType::Battlefield;
	mbIsSelectMode = true;
}

void UUISelectModesWidget::clickedRainbowSix()
{
	mBtnBattlefield->SetIsEnabled(true);
	mBtnRainbowSix->SetIsEnabled(false);
	mBtnTarkov->SetIsEnabled(true);
	mSelectedModes.game = EGameType::RainbowSix;
	mbIsSelectMode = true;
}

void UUISelectModesWidget::clickedTarkov()
{
	mBtnBattlefield->SetIsEnabled(true);
	mBtnRainbowSix->SetIsEnabled(true);
	mBtnTarkov->SetIsEnabled(false);
	mSelectedModes.game = EGameType::Tarkov;
	mbIsSelectMode = true;
}

void UUISelectModesWidget::clickedPlateType()
{
	mBtnPlateType->SetIsEnabled(false);
	mBtnCharacterType->SetIsEnabled(true);
	mbIsCharacterType = false;
	mbIsSelectTartgetType = true;
}

void UUISelectModesWidget::clickedCharacterType()
{
	mBtnPlateType->SetIsEnabled(true);
	mBtnCharacterType->SetIsEnabled(false);
	mbIsCharacterType = true;
	mbIsSelectTartgetType = true;
}

void UUISelectModesWidget::clickedStart()
{
	if(mbCanClickStart == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("not bind wait "));
		return;
	}
	if( mbIsSelectWeapon &&
		mbIsSelectOptic &&
		mbIsSelectMode &&
		mbIsSelectTartgetType
		)
	{

		auto playerController = Cast<ACharacterPlayerController>(GetOwningPlayer());
		mOnGameData.Execute(mSelectedModes);
		playerController->ShowInGameHUDAndStartTimer();
		playerController->GetTargetManager()->SetTargetType(mbIsCharacterType);
		RemoveFromParent();
		UE_LOG(LogTemp, Warning, TEXT("start "));
	}
	else
	{
		mBtnStart->SetColorAndOpacity(FLinearColor::Red);

		// need select all button
		UE_LOG(LogTemp, Warning, TEXT("Need select "));
	}
}
