// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "Blueprint/UserWidget.h"
#include "UIHUDWidget.generated.h"

enum class EHitType : uint8;
enum class EWaeponFireMode : uint8;
class UTextBlock;


/*
 * HUD 클래스입니다.
 * PlayerState의 값을 통해 화면에 정보를 보여줍니다.
 */

UCLASS()
class VERSION_API UUIHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	void InitializeWidgets();

	UFUNCTION()
	void UpdateRemainingTime(int32 remainingTime);
	UFUNCTION()
	void UpdateAmmo(int32 newAmmo);
	UFUNCTION()
	void UpdateAccuracy(int32 accuracy);
	UFUNCTION()
	void UpdateFireMode(EWaeponFireMode newFireMode);
	UFUNCTION()
	void UpdateScore(int32 score);
	UFUNCTION()
	void UpdateGameMode(FString newGameMode);

	UFUNCTION()
	void SetCrosshairVisibility(ESlateVisibility option);

	UFUNCTION()
	void AddViewPortHitMark(EHitType hitType);

	void clearHitMark();

protected:

	virtual void NativeConstruct() override;

protected:

	UPROPERTY()
	UTextBlock* RemainingTime;
	UPROPERTY()
	UTextBlock* Accuracy;
	UPROPERTY()
	UTextBlock* FireMode;
	UPROPERTY()
	UTextBlock* RemainingAmmo;
	UPROPERTY()
	UTextBlock* Score;
	UPROPERTY()
	UTextBlock* DisplayGameMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UUserWidget> mCrossHairClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UUserWidget> mHitMarkClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UUserWidget> mHeadshotMarkClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UUserWidget> mKillMarkClass;

	UPROPERTY()
	UUserWidget* mCrossHair;
	UPROPERTY()
	UUserWidget* mHitMark;
	UPROPERTY()
	UUserWidget* mHeadshotMark;
	UPROPERTY()
	UUserWidget* mKillMark;

	EHitType mCurrentMark;
	FTimerHandle mHitMarkTimer;
};
