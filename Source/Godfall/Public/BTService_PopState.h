// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallBTServiceBase.h"
#include "BTService_PopState.generated.h"

/**
 * 
 */
UCLASS()
class GODFALL_API UBTService_PopState : public UGodfallBTServiceBase
{
	GENERATED_BODY()
	
public:
	UBTService_PopState();

protected:
	// 이 서비스로 진입할 때 호출됩니다.
	virtual void OnSearchStartEvent(FBehaviorTreeSearchData& SearchData) override;

	// 서비스가 시작될 때 호출됩니다.
	virtual void OnBecomeRelevantEvent(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 서비즈가 중단될 때 호출됩니다.
	virtual void OnCeaseRelevantEvent(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// Interval 마다 호출됩니다.
	virtual void TickNodeEvent(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	void PopState(UBehaviorTreeComponent& OwnerComp);

private:
	UPROPERTY(EditAnywhere, DisplayName = "State Table Key")
	FBlackboardKeySelector mStateTableKey;

	UPROPERTY(EditAnywhere, DisplayName = "Pop Target State Key")
	FBlackboardKeySelector mPopTargetStateKey;

	UPROPERTY(EditAnywhere, DisplayName = "Target State Names")
	TArray<FName> mTargetStateNames;

	UPROPERTY(EditAnywhere, DisplayName = "Use Prioirty")
	bool mUsePriority = false;
};
