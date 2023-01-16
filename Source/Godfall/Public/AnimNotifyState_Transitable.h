// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallAnimNotifyStateBase.h"
#include "AnimNotifyState_Transitable.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Transitiable")
class GODFALL_API UAnimNotifyState_Transitable : public UGodfallAnimNotifyStateBase
{
	GENERATED_BODY()

public:
	UAnimNotifyState_Transitable();

public:
	virtual FName GetFunctionName() const override { return TEXT("AnimNotifyState_Transitable"); }
};
