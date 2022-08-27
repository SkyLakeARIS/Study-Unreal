// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "Blueprint/UserWidget.h"
#include "UISelectWidget.generated.h"

class UButton;

/*
 *  맵을 선택하는 UI 클래스입니다.
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
	UButton* BtnShortRange;
	UPROPERTY()
	UButton* BtnMidRange;
	UPROPERTY()
	UButton* BtnLongRange;

	UPROPERTY()
	UButton* BtnMovableShortRange;
	UPROPERTY()
	UButton* BtnMovableMidRange;
	UPROPERTY()
	UButton* BtnMovableLongRange;

};
