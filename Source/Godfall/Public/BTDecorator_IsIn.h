// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallBTDecoratorBase.h"
#include "BTDecorator_IsIn.generated.h"

UENUM(BlueprintType)
enum class EIsInConeDirectionMode : uint8
{
	ActorForward,
	ToTarget,
};

/**
 * 
 */
UCLASS()
class GODFALL_API UBTDecorator_IsIn : public UGodfallBTDecoratorBase
{
	GENERATED_BODY()
	
public:
	UBTDecorator_IsIn();

protected:
	virtual bool CalculateCondition(UBehaviorTreeComponent& OwnerComp) const override;

private:
	bool GetPivotLocation(const UBlackboardComponent* blackboardComp, FVector& location) const;
	bool GetTargetLocation(const UBlackboardComponent* blackboardComp, FVector& location) const;

public:
	UPROPERTY(EditAnywhere, DisplayName = "Pivot Key")
	FBlackboardKeySelector mPivotKey;

	UPROPERTY(EditAnywhere, DisplayName = "Target Key")
	FBlackboardKeySelector mTargetKey;

	UPROPERTY(EditAnywhere, DisplayName = "Radius")
	float mRadius = 500.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Is cone")
	bool mIsCone = false;

	UPROPERTY(EditAnywhere, DisplayName = "Half Angle", Meta = (ClampMin = 0.1f, ClampMax = 90.0f, EditCondition = "mIsCone == true", EditConditionHides))
	float mHalfAngle = 45.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Cone direction mode", Meta = (EditCondition = "mIsCone == true", EditConditionHides))
	EIsInConeDirectionMode mConeDirectionMode = EIsInConeDirectionMode::ActorForward;
};
