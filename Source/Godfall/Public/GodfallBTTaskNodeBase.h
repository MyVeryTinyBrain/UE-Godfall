// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GodfallBTTaskNodeBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class GODFALL_API UGodfallBTTaskNodeBase : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UGodfallBTTaskNodeBase();
	
protected:
	// 태스크가 시작될 때 호출됩니다.
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 태스크가 중단될 때 호출됩니다.
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 태스크가 시작된 후 진행중이라면 매 틱마다 호출됩니다.
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// 메세지를 받았을 때 호출됩니다.
	virtual void OnMessage(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, FName Message, int32 RequestID, bool bSuccess) override;

	// 태스크가 중단되면 호출됩니다.
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

public:
	bool IsValidKey(const class UBlackboardComponent* blackboardComp, const FBlackboardKeySelector& keySelector) const;
};
