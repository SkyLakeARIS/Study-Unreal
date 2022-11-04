// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "GameModeData.h"
#include "Version.h"
#include "Blueprint/UserWidget.h"
#include "UIPauseWidget.generated.h"

enum class eScopeType : uint8;
enum class eAimingType : uint8;
class UCheckBox;
class UEditableText;
class UTextBlock;
class UButton;


/*
 * 일시정지시 표시되는 UI 입니다.
 * 인게임 설정을 할 수 있습니다.
 * 조준경별 마우스 감도, 조준 설정(hold, toggle)
 */

UCLASS()
class VERSION_API UUIPauseWidget final : public UUserWidget
{
	GENERATED_BODY()

public:

	void UpdateInfoWhenOpen();

protected:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnResumeClicked();
	UFUNCTION()
	void OnRetryClicked();
	UFUNCTION()
	void OnReturnMenuClicked();

	UFUNCTION()
	void onChangeHip_X(const FText& InText, ETextCommit::Type InCommitType);
	UFUNCTION()
	void onChangeHip_Y(const FText& InText, ETextCommit::Type InCommitType);

	UFUNCTION()
	void onChangeScope1X_X(const FText& InText, ETextCommit::Type InCommitType);
	UFUNCTION()
	void onChangeScope1X_Y(const FText& InText, ETextCommit::Type InCommitType);

	UFUNCTION()
	void onChangeScope2dot5X_X(const FText& InText, ETextCommit::Type InCommitType);
	UFUNCTION()
	void onChangeScope2dot5X_Y(const FText& InText, ETextCommit::Type InCommitType);

	UFUNCTION()
	void onChangeScope6X_X(const FText& InText, ETextCommit::Type InCommitType);
	UFUNCTION()
	void onChangeScope6X_Y(const FText& InText, ETextCommit::Type InCommitType);

	UFUNCTION()
	void onHoldClicked();
	UFUNCTION()
	void onToggleClicked();

	UFUNCTION()
	void onDebugChecked(bool active);

	bool StringToInteger(const FText& InText, int32& outInt);

private:

	void updateAimSetting();
	void updateMouseSetting();

protected:
	
	UPROPERTY()
	UButton* mBtnResume;
	UPROPERTY()
	UButton* mBtnReturnMenu;
	UPROPERTY()
	UButton* mBtnRetry;

	UPROPERTY()
	UEditableText* mHipX;
	UPROPERTY()
	UEditableText* mHipY;
	UPROPERTY()
	UEditableText* mScope1X_X;
	UPROPERTY()
	UEditableText* mScope1X_Y;
	UPROPERTY()
	UEditableText* mScope2dot5X_X;
	UPROPERTY()
	UEditableText* mScope2dot5X_Y;
	UPROPERTY()
	UEditableText* mScope6X_X;
	UPROPERTY()
	UEditableText* mScope6X_Y;

	UPROPERTY()
	UButton* mBtnHold;
	UPROPERTY()
	UButton* mBtnToggle;

	UPROPERTY()
	UTextBlock* mMessageBox;
	UPROPERTY()
	UCheckBox* mDebugMode;

	FMouseSensitivity mMouseSetting;
	eAimingType mAimingType;
	bool mbCheckDebugMode;


	const int32 MIN_SETTING_VALUE = 10;
	const int32 MAX_SETTING_VALUE = 100;
};
