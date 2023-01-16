// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallEnemyAnimInstanceBase.h"
#include "AnimInstanceEarthAberrant.generated.h"

/**
 * 
 */
UCLASS()
class GODFALL_API UAnimInstanceEarthAberrant : public UGodfallEnemyAnimInstanceBase
{
	GENERATED_BODY()

public:
	UAnimInstanceEarthAberrant();

protected:
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds);
};
