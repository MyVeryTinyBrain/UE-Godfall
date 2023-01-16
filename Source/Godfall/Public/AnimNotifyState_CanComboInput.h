// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GodfallAnimNotifyStateBase.h"
#include "AnimNotifyState_CanComboInput.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "CanComboInput")
class GODFALL_API UAnimNotifyState_CanComboInput : public UGodfallAnimNotifyStateBase
{
	GENERATED_BODY()
	
public:
	UAnimNotifyState_CanComboInput();

public:
	virtual FName GetFunctionName() const override { return TEXT("AnimNotifyState_CanComboInput"); };
};
