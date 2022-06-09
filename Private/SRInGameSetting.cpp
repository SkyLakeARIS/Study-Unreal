// Fill out your copyright notice in the Description page of Project Settings.


#include "SRInGameSetting.h"

USRInGameSetting::USRInGameSetting()
{
	mSlotName = TEXT("MouseSetting");
	mSlotIndex = 0;
	AimingType = EAimingType::Hold;

}

FString USRInGameSetting::GetSlotName() const
{
	return mSlotName;
}

int32 USRInGameSetting::GetSlotIndex() const
{
	return mSlotIndex;
}
