// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "GameModeData.h"
#include "Blueprint/UserWidget.h"
#include "UISelectModesWidget.generated.h"

class UButton;
/*
 *	���� �������� ������ ���Ӹ�带 �����ϴ� UI Ŭ�����Դϴ�.
 *	����, ���ذ�, ���Ӹ�� Ÿ��, Ÿ�� Ÿ��
 */
UCLASS()
class VERSION_API UUISelectModesWidget final : public UUserWidget
{
	GENERATED_BODY()

private:

	enum eSelectFlag : uint8
	{
		WEAPON_SELECT_BIT = 0x1,
		OPTIC_SELECT_BIT = 0x2,
		GAMEMODE_SELECT_BIT = 0x4,
		TARGET_SELECT_BIT = 0x8,
		SELECT_ALL_FLAG = 0x0f
	};

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

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

	/*
	 *  select data
	 */
	FGameModeData mSelectedModes;
	bool mbIsCharacterType;
	int8 mSelectionFlag;

};
