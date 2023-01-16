// Fill out your copyright notice in the Description page of Project Settings.


#include "GodfallBTServiceBase.h"
#include <BehaviorTree/BlackboardComponent.h>

UGodfallBTServiceBase::UGodfallBTServiceBase()
{
	//bNotifyTick = true;
	//bNotifyOnSearch = true;
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
}

void UGodfallBTServiceBase::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	Super::OnSearchStart(SearchData);

	if (!mExecuteOnSearchStart) return;
	OnSearchStartEvent(SearchData);
}

void UGodfallBTServiceBase::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (!mExecuteOnBecomeRelevant) return;
	OnBecomeRelevantEvent(OwnerComp, NodeMemory);
}

void UGodfallBTServiceBase::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	if (!mExecuteOnCeaseRelevant) return;
	OnCeaseRelevantEvent(OwnerComp, NodeMemory);
}

void UGodfallBTServiceBase::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (!mExecuteOnTick) return;
	TickNodeEvent(OwnerComp, NodeMemory, DeltaSeconds);
}

bool UGodfallBTServiceBase::IsValidKey(const UBlackboardComponent* blackboardComp, const FBlackboardKeySelector& keySelector) const
{
	return blackboardComp->IsValidKey(blackboardComp->GetKeyID(keySelector.SelectedKeyName));
}
