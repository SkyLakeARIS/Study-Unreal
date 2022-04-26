// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Version.h"
#include "Blueprint/UserWidget.h"
#include "UIPauseWidget.generated.h"

/**
 *   ESC�� ������ ����ϴ� Menu UI Ŭ���� - �������Ʈ ����
 */
UCLASS()
class VERSION_API UUIPauseWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UFUNCTION()
void OnResumeClicked();
	UFUNCTION()
void OnRetryClicked();
	UFUNCTION()
void OnReturnMenuClicked();
	
protected:

	virtual void NativeConstruct() override;
	
	UPROPERTY()
	class UButton* btnResume;

	UPROPERTY()
	class UButton* btnReturnMenu;

	UPROPERTY()
	class UButton* btnRetry;
};
