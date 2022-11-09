// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "Blueprint/UserWidget.h"
#include "UIHUDWidget.generated.h"

enum class EHitType : uint8;
enum class eWaeponFireMode : uint8;
class UTextBlock;


/*
 * HUD 클래스입니다.
 * PlayerState의 값을 통해 화면에 정보를 보여줍니다.
 */

UCLASS()
class VERSION_API UUIHUDWidget final : public UUserWidget
{
	GENERATED_BODY()
public:

	void InitializeWidgets();

	UFUNCTION()
	void UpdateRemainingTime(const int32 remainingTime);
	UFUNCTION()
	void UpdateAmmo(const int32 newAmmo);
	UFUNCTION()
	void UpdateAccuracy(const int32 accuracy);
	UFUNCTION()
	void UpdateFireMode(const eWaeponFireMode newFireMode);
	UFUNCTION()
	void UpdateScore(const int32 score);
	UFUNCTION()
	void UpdateGameMode(const FString newGameMode);

	UFUNCTION()
	void SetCrosshairVisibility(const ESlateVisibility option);

	UFUNCTION()
	void AddHitMarkToViewPort(const EHitType hitType);
	void RemoveHitMarkFromViewport();

protected:

	virtual void NativeConstruct() override;

	void hideHitMark();


protected:

	UPROPERTY()
	UTextBlock* mRemainingTime;
	UPROPERTY()
	UTextBlock* mAccuracy;
	UPROPERTY()
	UTextBlock* mFireMode;
	UPROPERTY()
	UTextBlock* mRemainingAmmo;
	UPROPERTY()
	UTextBlock* mScore;
	UPROPERTY()
	UTextBlock* mDisplayGameMode;

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

	UPROPERTY()
	UUserWidget* mCurrentMark;

	FTimerHandle mHitMarkTimer;
};
