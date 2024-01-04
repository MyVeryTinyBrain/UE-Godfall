// Fill out your copyright notice in the Description page of Project Settings.


#include "GodfallBTDecoratorBase.h"
#include <BehaviorTree/BlackboardComponent.h>

UGodfallBTDecoratorBase::UGodfallBTDecoratorBase()
{
	INIT_DECORATOR_NODE_NOTIFY_FLAGS();

	// 틱 노드 함수를 사용해 매 프레임마다 검사한다.
	bNotifyTick = true;
}

bool UGodfallBTDecoratorBase::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	return CalculateCondition(OwnerComp);
}

void UGodfallBTDecoratorBase::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	// 데코레이터 노드에 진입할 때 조건 검사 결과를 노드 메모리에 저장한다.
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
			// 검사 결과가 진입 시점의 조건 검사 결과와 다르면 실행 요청을 보내 데코레이터 노드가 다시 판단하게 한다.
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
