// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Version.h"
#include "Blueprint/UserWidget.h"
#include "SelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class VERSION_API USelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	virtual void NativeConstruct() override;

	UPROPERTY()
		class UButton* BtnDev;

	UPROPERTY()
	class UButton* BtnCombination;

private:
	UFUNCTION()
	void OnDevClicked();

	UFUNCTION()
	void OnCombinationClicked();

};
