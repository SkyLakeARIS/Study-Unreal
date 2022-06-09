// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterPlayerController.h"
#include "SRPlayerState.h"
#include "SRSpawnPoint.h"
#include "SRTargetManager.h"
#include "UIPauseWidget.h"
#include "UIHUDWidget.h"
#include "UIResultWidget.h"
#include "UISelectModesWidget.h"

ACharacterPlayerController::ACharacterPlayerController()
{
	static ConstructorHelpers::FClassFinder<UUIPauseWidget> UI_INGAMEMENU(TEXT("/Game/UI/UI_PauseMenu.UI_PauseMenu_C"));
	if (UI_INGAMEMENU.Succeeded())
	{
		mPauseWidgetClass = UI_INGAMEMENU.Class;
	}
	static ConstructorHelpers::FClassFinder<UUIHUDWidget> UI_INGAMEHUD(TEXT("/Game/UI/UI_HUD.UI_HUD_C"));
	if (UI_INGAMEHUD.Succeeded())
	{
		HUDClass = UI_INGAMEHUD.Class;
	}
	static ConstructorHelpers::FClassFinder<UUIResultWidget> UI_RESULT(TEXT("/Game/UI/UI_Result.UI_Result_C"));
	if (UI_RESULT.Succeeded())
	{
		mReusltWidgetClass = UI_RESULT.Class;
	}
	static ConstructorHelpers::FClassFinder<UUISelectModesWidget> UI_SELECTMODES(TEXT("/Game/UI/UI_SelectModes.UI_SelectModes_C"));
	if (UI_SELECTMODES.Succeeded())
	{
		mSelectModesWidgetClass = UI_SELECTMODES.Class;
	}
	remainingTime = 90;
	mTimeToReady = 5;
	mSensitivity = 45.0f;
	mbDebugMode = false;
}

void ACharacterPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void ACharacterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ACharacterPlayerController::ChangeInputMode(bool bIsGameMode)
{
	if (bIsGameMode)
	{
		SetInputMode(GameInputMode);
		bShowMouseCursor = false;
	}
	else
	{
		SetInputMode(UIInputMode);
		bShowMouseCursor = true;
	}
}

void ACharacterPlayerController::ResumeCountDown()
{
	GetWorld()->GetTimerManager().UnPauseTimer(THCountDown);
}

void ACharacterPlayerController::ClearCountDown()
{
	GetWorld()->GetTimerManager().ClearTimer(THCountDown);
}

void ACharacterPlayerController::BindStatToUI()
{
	mPlayerState->BindHUD(InGameHUD);
}

void ACharacterPlayerController::ShowInGameHUDAndStartTimer()
{
	ChangeInputMode(true);
	InGameHUD->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().SetTimer(THCountDown, this, &ACharacterPlayerController::countReadyTime, 1.0f, true, 0.0f);
}

void ACharacterPlayerController::SetDebugMode(bool active)
{
	mbDebugMode = active;
}

bool ACharacterPlayerController::IsDebugging() const
{
	return mbDebugMode;
}

ASRPlayerState* ACharacterPlayerController::GetPlayerState() const
{
	return mPlayerState;
}

UUIHUDWidget* ACharacterPlayerController::GetIngameHUD() const
{
	return InGameHUD;
}

UUISelectModesWidget* ACharacterPlayerController::GetSelectModesWidget() const
{
	return mSelectModesWidget;
}

ASRTargetManager* ACharacterPlayerController::GetTargetManager() const
{
	return mTargetManager;
}

void ACharacterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction(TEXT("Pause"), EInputEvent::IE_Pressed, this, &ACharacterPlayerController::PauseGame);



}

void ACharacterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(IsLocalController() && HUDClass)
	{
		InGameHUD = CreateWidget<UUIHUDWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), HUDClass);
	}

	if (IsLocalController() && mSelectModesWidgetClass)
	{
		mSelectModesWidget = CreateWidget<UUISelectModesWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), mSelectModesWidgetClass);
	}


	mPlayerState = Cast<ASRPlayerState>(PlayerState);

	ChangeInputMode(false);
	mSelectModesWidget->AddToViewport();
	InGameHUD->AddToViewport();
	InGameHUD->SetVisibility(ESlateVisibility::Hidden);

	BindStatToUI();
	mTargetManager = GetWorld()->SpawnActor<ASRTargetManager>(ASRTargetManager::StaticClass(), FVector::ZeroVector,  FRotator::ZeroRotator);
	//// test for checking target movement.
	//TArray<AActor*> list;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASRSpawnPoint::StaticClass(), list);
	//for(auto a : list)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"),a->GetActorLocation().X, a->GetActorLocation().Y, a->GetActorLocation().Z);
	//}

	//TArray<AActor*> targetlist;

	//UGameplayStatics::GetAllActorsOfClass(GetWorld(),ATargetCharacter::StaticClass(), targetlist);
	//auto b = Cast<ATargetCharacter>(targetlist[0]);
	//b->SetIsMovable(true);
	//b->SetEndLocation(list[1]->GetActorLocation());

	//auto c = Cast<ATargetCharacter>(targetlist[1]);
	//c->SetIsMovable(true);
	//c->SetEndLocation(list[0]->GetActorLocation());
	//

}

void ACharacterPlayerController::PauseGame()
{
	UE_LOG(LogTemp, Warning, TEXT("GamePuaue"));
	InGameHUD->SetVisibility(ESlateVisibility::Hidden);
	mPauseWidget = CreateWidget<UUIPauseWidget>(this, mPauseWidgetClass);
	mPauseWidget->AddToViewport(3);
	mPauseWidget->UpdateInfoWhenOpen();
	SetPause(true);
	ChangeInputMode(false);
}


void ACharacterPlayerController::countDownMainTime()
{
	--remainingTime;
	InGameHUD->UpdateRemainingTime(remainingTime);
	if(remainingTime <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(THCountDown);
		result();
	}
}

void ACharacterPlayerController::countReadyTime()
{
	--mTimeToReady;
	InGameHUD->UpdateRemainingTime(mTimeToReady);
	if (mTimeToReady <= 0)
	{
		mTargetManager->RandomTargetSpawn();
		mTargetManager->RandomTargetSpawn();

		GetWorld()->GetTimerManager().ClearTimer(THCountDown);
		GetWorld()->GetTimerManager().SetTimer(THCountDown, this, &ACharacterPlayerController::countDownMainTime, 1.0f, true, 0.0f);
	}
}

void ACharacterPlayerController::result()
{
	InGameHUD->SetVisibility(ESlateVisibility::Hidden);
	mResultWidget = CreateWidget<UUIResultWidget>(this, mReusltWidgetClass);
	mResultWidget->AddToViewport(3);
	mResultWidget->UpdateStageInfo(mPlayerState);
	ChangeInputMode(false);
	SetPause(true);
}
