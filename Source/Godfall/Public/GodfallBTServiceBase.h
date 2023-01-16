// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "BehaviorTree/BTService.h"
#include "GodfallBTServiceBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class GODFALL_API UGodfallBTServiceBase : public UBTService
{
	GENERATED_BODY()

public:
	UGodfallBTServiceBase();

protected:
	// 이 서비스로 진입할 때 호출됩니다.
	virtual void OnSearchStartEvent(FBehaviorTreeSearchData& SearchData) {}

	// 서비스가 시작될 때 호출됩니다.
	virtual void OnBecomeRelevantEvent(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {}

	// 서비즈가 중단될 때 호출됩니다.
	virtual void OnCeaseRelevantEvent(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {}

	// Interval 마다 호출됩니다.
	virtual void TickNodeEvent(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {}

protected:
	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) final override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) final override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) final override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) final override;

public:
	bool IsValidKey(const class UBlackboardComponent* blackboardComp, const FBlackboardKeySelector& keySelector) const;
	
protected:
	UPROPERTY(EditAnywhere, DisplayName = "Edit Execute flags", Meta = (EditCondition = "false", EditConditionHides))
	bool mEditExecuteFlags = true;

	UPROPERTY(EditAnywhere, DisplayName = "Execute on Search Start", Meta = (EditCondition = "mEditExecuteFlags == true"))
	bool mExecuteOnSearchStart = true;

	UPROPERTY(EditAnywhere, DisplayName = "Execute on Tick", Meta = (EditCondition = "mEditExecuteFlags == true"))
	bool mExecuteOnTick = true;

	UPROPERTY(EditAnywhere, DisplayName = "Execute on Become Relevant", Meta = (EditCondition = "mEditExecuteFlags == true"))
	bool mExecuteOnBecomeRelevant = true;

	UPROPERTY(EditAnywhere, DisplayName = "Execute on Cease Relevant", Meta = (EditCondition = "mEditExecuteFlags == true"))
	bool mExecuteOnCeaseRelevant = true;
};
