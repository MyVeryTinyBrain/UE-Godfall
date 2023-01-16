// Fill out your copyright notice in the Description page of Project Settings.


#include "GodfallBTTaskNodeBase.h"
#include <BehaviorTree/BlackboardComponent.h>

UGodfallBTTaskNodeBase::UGodfallBTTaskNodeBase()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UGodfallBTTaskNodeBase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//return Super::ExecuteTask(OwnerComp, NodeMemory);
	return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UGodfallBTTaskNodeBase::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//return Super::AbortTask(OwnerComp, NodeMemory);
	return EBTNodeResult::Aborted;
}

void UGodfallBTTaskNodeBase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	//Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

void UGodfallBTTaskNodeBase::OnMessage(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, FName Message, int32 RequestID, bool bSuccess)
{
	Super::OnMessage(OwnerComp, NodeMemory, Message, RequestID, bSuccess);
}

void UGodfallBTTaskNodeBase::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	//Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

bool UGodfallBTTaskNodeBase::IsValidKey(const UBlackboardComponent* blackboardComp, const FBlackboardKeySelector& keySelector) const
{
	return blackboardComp->IsValidKey(blackboardComp->GetKeyID(keySelector.SelectedKeyName));
}
