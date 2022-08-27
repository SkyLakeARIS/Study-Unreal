// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "GameFramework/PlayerController.h"
#include "UIPlayerController.generated.h"


/*
 * 블루프린트 UI위젯에 사용되는 플레이어 컨트롤러 클래스입니다.
 */

UCLASS()
class VERSION_API AUIPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=UI)
    TSubclassOf<UUserWidget> UIWidgetClass;
	
	UPROPERTY()
	UUserWidget* UIWidgetInstance;	// esc key == pause game

};
