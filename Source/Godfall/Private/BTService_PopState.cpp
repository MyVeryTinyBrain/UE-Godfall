// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_PopState.h"
#include "BlackboardKeyType_StateTable.h"
#include <BehaviorTree/BlackboardComponent.h>
#include "GodfallUtil.h"

UBTService_PopState::UBTService_PopState()
{
	NodeName = TEXT("Pop State");

	//mStateTableKey.AddObjectFilter(this, NAME_None, UBlackboardKeyType_StateTable::StaticClass());

	const FString FilterName = FName(NAME_None).ToString() + TEXT("_StateTable");
	mStateTableKey.AllowedTypes.Add(NewObject<UBlackboardKeyType_StateTable>(this, *FilterName));

	mPopTargetStateKey.AddNameFilter(this, NAME_None);

	mEditExecuteFlags = true;
	mExecuteOnSearchStart = true;
	mExecuteOnBecomeRelevant = false;
	mExecuteOnCeaseRelevant = false;
	mExecuteOnTick = false;
}

void UBTService_PopState::OnSearchStartEvent(FBehaviorTreeSearchData& SearchData)
{
	PopState(SearchData.OwnerComp);
}

void UBTService_PopState::OnBecomeRelevantEvent(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	PopState(OwnerComp);
}

void UBTService_PopState::OnCeaseRelevantEvent(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	PopState(OwnerComp);
}

void UBTService_PopState::TickNodeEvent(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	PopState(OwnerComp);
}

void UBTService_PopState::PopState(UBehaviorTreeComponent& OwnerComp)
{	
	UBlackboardComponent* blackboardComp = OwnerComp.GetBlackboardComponent();
	FStateTable stateTable = blackboardComp->GetValue<UBlackboardKeyType_StateTable>(mStateTableKey.SelectedKeyName);
	TArray<FName> targetStateNames = mTargetStateNames;
#pragma region SHUFFLE STATE NAMES
	if (!mUsePriority)
	{
		GodfallUtil::Sort::Mix(targetStateNames, targetStateNames.Num());
	}
#pragma endregion
	int32 numContains = 0, numSetSucceeded = 0;
	for (const auto& name : targetStateNames)
	{
		bool containsName;
		bool setSucceeded = stateTable.TrySetAvailable(name, false, containsName);
		ensureMsgf(containsName, TEXT("The state name \"%s\" does not exist in the state table."), *name.ToString());
		if (containsName) ++numContains;
		if (setSucceeded) ++numSetSucceeded;
		// 하나의 상태 변경에 성공하면 종료합니다.
		if (containsName && setSucceeded)
		{
			uint32 numAvailable = 0, numStates = 0;
			for (auto& state : stateTable.States)
			{
				numAvailable += state.Available ? 1 : 0;
				++numStates;
			}
			blackboardComp->SetValue<UBlackboardKeyType_StateTable>(mStateTableKey.SelectedKeyName, stateTable);
			blackboardComp->SetValueAsName(mPopTargetStateKey.SelectedKeyName, name);
			return;
		}
	}
	// 상태 이름은 유효하나 설정 가능한 모든 상태들이 비활성화 상태이면
	// 등록한 배열에 존재하는 상태들을 찾아 활성화 시키고 다시 한 번 상태를 뽑습니다.
	// 이 때 등록하지 않은 상태는 활성화하지 않습니다.
	// 따라서 필요한 상태만 지속적으로 뽑아낼 수 있게 됩니다.
	if (numContains > 0 && numSetSucceeded == 0)
	{
		for (const auto& name : targetStateNames)
		{
			bool containsName;
			stateTable.SetAvailable(name, true, containsName);
			blackboardComp->SetValue<UBlackboardKeyType_StateTable>(mStateTableKey.SelectedKeyName, stateTable);
		}
		PopState(OwnerComp);
	}
}
