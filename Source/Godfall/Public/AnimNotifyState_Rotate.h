// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RotateTypes.h"
#include "GodfallAnimNotifyStateBase.h"
#include "AnimNotifyState_Rotate.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Rotate")
class GODFALL_API UAnimNotifyState_Rotate : public UGodfallAnimNotifyStateBase
{
	GENERATED_BODY()
	
public:
	UAnimNotifyState_Rotate();

public:
	virtual FName GetFunctionName() const override { return TEXT("AnimNotifyState_Rotate"); };

	const FRotateOverride& GetOverrideData() const { return Data; }
	const FRotateOverride& GetNoOverrideData() const { return FRotateOverride::NoOverride(); }

private:
	UPROPERTY(EditAnywhere, DisplayName = "Data", Meta = (AllowPrivateAccess))
	FRotateOverride Data = FRotateOverride(true);
};
