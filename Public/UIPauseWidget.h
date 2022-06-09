// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Version.h"
#include "Blueprint/UserWidget.h"
#include "UIPauseWidget.generated.h"

class UCheckBox;
class UEditableText;
class UTextBlock;
class UButton;
/**
 *   ESC�� ������ ����ϴ� Menu UI Ŭ���� - �������Ʈ ����
 */
UCLASS()
class VERSION_API UUIPauseWidget : public UUserWidget
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
	void onChangeHipX(const FText& InText, ETextCommit::Type InCommitType);
	UFUNCTION()
	void onChangeHipY(const FText& InText, ETextCommit::Type InCommitType);

	UFUNCTION()
	void onChangeOpticX(const FText& InText, ETextCommit::Type InCommitType);
	UFUNCTION()
	void onChangeOpticY(const FText& InText, ETextCommit::Type InCommitType);

	UFUNCTION()
	void onChangeACOGX(const FText& InText, ETextCommit::Type InCommitType);
	UFUNCTION()
	void onChangeACOGY(const FText& InText, ETextCommit::Type InCommitType);

	UFUNCTION()
	void onChangeScopeX(const FText& InText, ETextCommit::Type InCommitType);
	UFUNCTION()
	void onChangeScopeY(const FText& InText, ETextCommit::Type InCommitType);

	UFUNCTION()
	void onHoldClicked();
	UFUNCTION()
	void onToggleClicked();

	UFUNCTION()
	void onDebugChecked(bool active);

	bool StringToInteger(const FText& InText, int32* int_Out);

protected:
	
	UPROPERTY()
	UButton* btnResume;
	UPROPERTY()
	UButton* btnReturnMenu;
	UPROPERTY()
	UButton* btnRetry;

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
	UButton* btnHold;
	UPROPERTY()
	UButton* btnToggle;

	UPROPERTY()
	UTextBlock* mMessageBox;
	UPROPERTY()
	UCheckBox* mDebugMode;

	bool mbCheckDebugMode;
};
