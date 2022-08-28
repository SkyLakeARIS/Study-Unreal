// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "Blueprint/UserWidget.h"
#include "UISelectWidget.generated.h"

class UButton;

/*
 *  ���� �����ϴ� UI Ŭ�����Դϴ�.
 */
UCLASS()
class VERSION_API USelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void OnShortRangeClicked();

	UFUNCTION(BlueprintCallable)
	void OnMidRangeClicked();

	UFUNCTION(BlueprintCallable)
	void OnLongRangeClicked();


	UFUNCTION(BlueprintCallable)
	void OnMovableShortRangeClicked();

	UFUNCTION(BlueprintCallable)
	void OnMovableMidRangeClicked();

	UFUNCTION(BlueprintCallable)
	void OnMovableLongRangeClicked();

protected:

	UPROPERTY()
	UButton* mBtnShortRange;
	UPROPERTY()
	UButton* mBtnMidRange;
	UPROPERTY()
	UButton* mBtnLongRange;

	UPROPERTY()
	UButton* mBtnMovableShortRange;
	UPROPERTY()
	UButton* mBtnMovableMidRange;
	UPROPERTY()
	UButton* mBtnMovableLongRange;

};
