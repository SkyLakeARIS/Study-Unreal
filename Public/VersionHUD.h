// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 
#include "Version.h"
#include "GameFramework/HUD.h"
#include "VersionHUD.generated.h"

UCLASS()
class AVersionHUD : public AHUD
{
	GENERATED_BODY()

public:
	AVersionHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

