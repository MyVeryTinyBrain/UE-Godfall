// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallBTTaskNodeBase.h"
#include "GodfallEnemyEnums.h"
#include "BTTask_MoveToTarget.generated.h"

/**
 * 
 */
UCLASS()
class GODFALL_API UBTTask_MoveToTarget : public UGodfallBTTaskNodeBase
{
	GENERATED_BODY()

public:
	UBTTask_MoveToTarget();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
	bool GetTargetLocation(const UBlackboardComponent* blackboardComp, FVector& location) const;
	class AGodfallEnemyBase* GetEnemy(UBehaviorTreeComponent& OwnerComp) const;
	const AActor* GetTargetActor(const UBlackboardComponent* blackboardComp) const;

private:
	UPROPERTY(EditAnywhere, DisplayName = "Target Key")
	FBlackboardKeySelector mTargetKey;

	UPROPERTY(EditAnywhere, DisplayName = "Move Speed")
	EEnemyMoveType mMoveType = EEnemyMoveType::Run;
	
	UPROPERTY(EditAnywhere, DisplayName = "Walk Direction", Meta = (EditCondition = "mMoveType == EEnemyMoveType::Walk", EditConditionHides))
	E4Direction mWalkDirection = E4Direction::Forward;

	UPROPERTY(EditAnywhere, DisplayName = "Stop if partial path")
	bool mStopIfPartialPath = true;

	UPROPERTY(EditAnywhere, DisplayName = "Look On Changed Move State")
	bool mLookOnChangedMoveState = true;

	UPROPERTY(EditAnywhere, DisplayName = "Use Wait")
	bool mUseWait = true;

	UPROPERTY(EditAnywhere, DisplayName = "Look On Wait", Meta = (EditCondition = "mUseWait == true", EditConditionHides))
	bool mLookOnWait = true;

	UPROPERTY(EditAnywhere, DisplayName = "Block Distance", Meta = (ClampMin = 0.1f))
	float mBlockDistance = 100.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Stop Radius", Meta = (ClampMin = 0.1f))
	float mStopRadius = 100.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Use Avoid")
	bool mUseAvoid = true;

	UPROPERTY(EditAnywhere, DisplayName = "Avoid Distance", Meta = (EditCondition = "mUseAvoid == true", EditConditionHides, ClampMin = 0.1f))
	float mAvoidDistance = 450.0f;
};
