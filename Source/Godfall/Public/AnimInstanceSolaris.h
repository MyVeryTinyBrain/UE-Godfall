// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallEnemyAnimInstanceBase.h"
#include "AnimInstanceSolaris.generated.h"

/**
 * 
 */
UCLASS()
class GODFALL_API UAnimInstanceSolaris : public UGodfallEnemyAnimInstanceBase
{
	GENERATED_BODY()

public:
	UAnimInstanceSolaris();

protected:
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds);
};
