// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "BehaviorTree/BTDecorator.h"
#include "GodfallBTDecoratorBase.generated.h"

UENUM(BlueprintType)
enum class EUBTDecoratorExecuteMode : uint8
{
	OnBecome,
	Tick,
};

struct FUBTDecoratorMemory
{
	bool BeforeCondition;
};

/**
 * 
 */
UCLASS(Abstract)
class GODFALL_API UGodfallBTDecoratorBase : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UGodfallBTDecoratorBase();

	typedef FUBTDecoratorMemory TNodeInstanceMemory;

	virtual uint16 GetInstanceMemorySize() const override { return sizeof(TNodeInstanceMemory); }

protected:
	// 조건을 검사합니다.
	virtual bool CalculateCondition(UBehaviorTreeComponent& OwnerComp) const { return true; }

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	bool IsValidKey(const class UBlackboardComponent* blackboardComp, const FBlackboardKeySelector& keySelector) const;

protected:
	UPROPERTY(EditAnywhere, DisplayName = "Edit Execute Mode", Meta = (EditCondition = "false", EditConditionHides))
	bool mEditExecuteMode = true;

	UPROPERTY(EditAnywhere, DisplayName = "Execute Mode", Meta = (EditCondition = "mEditExecuteMode == true"))
	EUBTDecoratorExecuteMode mExecuteMode = EUBTDecoratorExecuteMode::Tick;
};
