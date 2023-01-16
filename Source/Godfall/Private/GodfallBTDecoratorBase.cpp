// Fill out your copyright notice in the Description page of Project Settings.


#include "GodfallBTDecoratorBase.h"
#include <BehaviorTree/BlackboardComponent.h>

UGodfallBTDecoratorBase::UGodfallBTDecoratorBase()
{
	INIT_DECORATOR_NODE_NOTIFY_FLAGS();

	bNotifyTick = true;
}

bool UGodfallBTDecoratorBase::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	return CalculateCondition(OwnerComp);
}

void UGodfallBTDecoratorBase::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	TNodeInstanceMemory* DecoratorMemory = CastInstanceNodeMemory<TNodeInstanceMemory>(NodeMemory);
	DecoratorMemory->BeforeCondition = CalculateRawConditionValue(OwnerComp, NodeMemory);
}

void UGodfallBTDecoratorBase::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
}

void UGodfallBTDecoratorBase::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	switch (mExecuteMode)
	{
		case EUBTDecoratorExecuteMode::Tick: 
		{
			TNodeInstanceMemory* DecoratorMemory = CastInstanceNodeMemory<TNodeInstanceMemory>(NodeMemory);
			bool condition = CalculateRawConditionValue(OwnerComp, NodeMemory);
			if (condition != DecoratorMemory->BeforeCondition)
			{
				DecoratorMemory->BeforeCondition = condition;
				OwnerComp.RequestExecution(this);
			}
		}
		break;
	}
}

bool UGodfallBTDecoratorBase::IsValidKey(const UBlackboardComponent* blackboardComp, const FBlackboardKeySelector& keySelector) const
{
	return blackboardComp->IsValidKey(blackboardComp->GetKeyID(keySelector.SelectedKeyName));
}
