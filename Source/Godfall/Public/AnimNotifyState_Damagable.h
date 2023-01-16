// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageTypes.h"
#include "GodfallAnimNotifyStateBase.h"
#include "AnimNotifyState_Damagable.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Damagable")
class GODFALL_API UAnimNotifyState_Damagable : public UGodfallAnimNotifyStateBase
{
	GENERATED_BODY()

public:
	UAnimNotifyState_Damagable();

public:
	virtual FName GetFunctionName() const override { return TEXT("AnimNotifyState_Damagable"); };

public:
	const FDamagableData& GetData() const { return mDamagableData; }

private:
	UPROPERTY(EditAnywhere, DisplayName = "Damagable Data", Meta = (AllowPrivateAccess))
	FDamagableData mDamagableData;
};
