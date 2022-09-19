// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "GameModeData.h"
#include "Blueprint/UserWidget.h"
#include "UISelectModesWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnInitGameData, FGameModeData)

class UButton;
/*
 *	���� �������� ������ ���Ӹ�带 �����ϴ� UI Ŭ�����Դϴ�.
 *	����, ���ذ�, ���Ӹ�� Ÿ��, Ÿ�� Ÿ��
 */
UCLASS()
class VERSION_API UUISelectModesWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void BindCharacterInfo(class ASRPlayerCharacter* character);

protected:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void clickedAR();
	UFUNCTION()
	void clickedHG();
	UFUNCTION()
	void clickedSR();

	UFUNCTION()
	void clicked1X();
	UFUNCTION()
	void clicked2Dot5X();
	UFUNCTION()
	void clicked6X();

	UFUNCTION()
	void clickedBattlefield();
	UFUNCTION()
	void clickedRainbowSix();
	UFUNCTION()
	void clickedTarkov();

	UFUNCTION()
	void clickedPlateType();
	UFUNCTION()
	void clickedCharacterType();

	UFUNCTION()
	void clickedStart();


protected:
	UPROPERTY()
	UButton* mBtnAR;
	UPROPERTY()
	UButton* mBtnHG;
	UPROPERTY()
	UButton* mBtnSR;

	UPROPERTY()
	UButton* mBtn1X;
	UPROPERTY()
	UButton* mBtn2Dot5X;
	UPROPERTY()
	UButton* mBtn6X;

	UPROPERTY()
	UButton* mBtnBattlefield;
	UPROPERTY()
	UButton* mBtnRainbowSix;
	UPROPERTY()
	UButton* mBtnTarkov;

	UPROPERTY()
	UButton* mBtnPlateType;
	UPROPERTY()
	UButton* mBtnCharacterType;

	UPROPERTY()
	UButton* mBtnStart;

	FOnInitGameData mOnGameData;
	FGameModeData mSelectedModes;
	bool mbIsCharacterType;

	bool mbCanClickStart;
	bool mbIsSelectWeapon;
	bool mbIsSelectOptic;
	bool mbIsSelectMode;
	bool mbIsSelectTartgetType;
};
