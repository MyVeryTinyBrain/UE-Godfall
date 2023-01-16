// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GodfallAnimNotifyStateBase.h"
#include "AnimNotifyState_Exitable.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Exitable")
class GODFALL_API UAnimNotifyState_Exitable : public UGodfallAnimNotifyStateBase
{
	GENERATED_BODY()
	
public:
	UAnimNotifyState_Exitable();

public:
	virtual FName GetFunctionName() const override { return TEXT("AnimNotifyState_Exitable"); }

};
