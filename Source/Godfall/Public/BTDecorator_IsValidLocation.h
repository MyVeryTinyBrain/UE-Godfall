// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallBTDecoratorBase.h"
#include "BTDecorator_IsValidLocation.generated.h"

/**
 * 
 */
UCLASS()
class GODFALL_API UBTDecorator_IsValidLocation : public UGodfallBTDecoratorBase
{
	GENERATED_BODY()

public:
	UBTDecorator_IsValidLocation();

protected:
	virtual bool CalculateCondition(UBehaviorTreeComponent& OwnerComp) const override;

public:
	UPROPERTY(EditAnywhere, DisplayName = "Location")
	FBlackboardKeySelector mLocationKey;
};
