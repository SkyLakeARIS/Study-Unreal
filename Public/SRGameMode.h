// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Version.h"
#include "GameModeData.h"
#include "GameFramework/GameModeBase.h"
#include "SRGameMode.generated.h"

class UUIHUDWidget;
class ASRPlayerController;
class ASRTargetManager;
class ASRPlayerState;

/**
 *  맵을 선택할 때의 게임모드입니다.
 *	블루프린트로 상속하여 구현합니다.
 */
UCLASS(BlueprintType, Blueprintable)
class VERSION_API ASRGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ASRGameMode();

	void PauseGame();
	void ResumeGame();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void BeginPlay() override;
	virtual void StartPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/*
	 *  getter
	 */
	EScopeType GetScopeType() const;
	UUIHUDWidget* GetHUDWidget() const;
	ASRPlayerState* GetPlayerState() const;
	bool IsDebugMode() const;

	/*
	 *  setter
	 */
	void SettingGameAndStartGame();
	void SetGameModeData(FGameModeData& modeData, bool isCharacterType);
	void SetDebugMode(bool isActive);

private:

	void countdownReadyGameTimer();
	void countdownMainGameTimer();

	void startMainGame();
	void endMainGame();

protected:

	UPROPERTY(BlueprintReadWrite)
	ASRPlayerState* mPlayerState;
	UPROPERTY()
	ASRPlayerController* mPlayerController;
	UPROPERTY()
	ASRTargetManager* mTargetManager;

	/*
	 *  ui
	 */
	UPROPERTY()
	UUIHUDWidget* mHUD;

	/*
	 *  game info data
	 */
	UPROPERTY(BlueprintReadWrite)
	EGameModeType mGameModeType;
private:
	/*
	 *  timer
	 */
	FTimerHandle mTimer;
	int32 mGameTime;
	int32 mReadyTime;

	/*
	 *  game info data
	 */
	FGameModeData mModeData;

	/*
	 *  debug
	 */
	bool mbDebugMode;
};


