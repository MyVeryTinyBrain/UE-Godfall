// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallBTTaskNodeBase.h"
#include "BTTaskNode_WalkTo.generated.h"

struct FUBTTaskNode_WalkTo_Memory
{
	E4Direction InitialDirection = E4Direction::Backward;
};

/**
 * 
 */
UCLASS()
class GODFALL_API UBTTaskNode_WalkTo : public UGodfallBTTaskNodeBase
{
	GENERATED_BODY()

public:
	UBTTaskNode_WalkTo();

	typedef FUBTTaskNode_WalkTo_Memory TNodeInstanceMemory;
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(TNodeInstanceMemory); }

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
	bool GetTargetLocation(const UBlackboardComponent* blackboardComp, FVector& location) const;
	class AGodfallEnemyBase* GetEnemy(UBehaviorTreeComponent& OwnerComp) const;
	const AActor* GetTargetActor(const UBlackboardComponent* blackboardComp) const;
	bool IsNearWithTarget(UBehaviorTreeComponent& OwnerComp) const;
	bool GetWalkDirection(UBehaviorTreeComponent& OwnerComp, E4Direction direction, FVector& vector) const;

private:
	UPROPERTY(EditAnywhere, DisplayName = "Target Key")
	FBlackboardKeySelector mTargetKey;

	UPROPERTY(EditAnywhere, DisplayName = " Use Stop Radius With Near Target")
	bool mUseStopRadiusWithNearTarget = true;

	UPROPERTY(EditAnywhere, DisplayName = "Stop Radius With Near Target", Meta = (EditCondition = "mUseStopRadiusWithNearTarget == true", EditConditionHides, ClampMin = 0.1f))
	float mStopRadiusWithNearTarget = 300.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Stop Radius", Meta = ( ClampMin = 0.1f))
	float mStopRadius = 100.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Walk Distance", Meta = (ClampMin = 0.1f))
	float mWalkDistance = 300.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Walk Direction", Meta = (EditCondition = "mUseRandomWalkDirection != true", EditConditionHides))
	E4Direction mWalkDirection = E4Direction::Backward;

	UPROPERTY(EditAnywhere, DisplayName = "Use Random Walk Direction")
	bool mUseRandomWalkDirection = false;

	UPROPERTY(EditAnywhere, DisplayName = "Allow Forward Direction", Meta = (EditCondition = "mUseRandomWalkDirection == true", EditConditionHides))
	bool mAllowForwardDirection = false;
	
	UPROPERTY(EditAnywhere, DisplayName = "Allow Backward Direction", Meta = (EditCondition = "mUseRandomWalkDirection == true", EditConditionHides))
	bool mAllowBackwardDirection = true;
	
	UPROPERTY(EditAnywhere, DisplayName = "Allow Left Direction", Meta = (EditCondition = "mUseRandomWalkDirection == true", EditConditionHides))
	bool mAllowLeftDirection = true;
	
	UPROPERTY(EditAnywhere, DisplayName = "Allow Right Direction", Meta = (EditCondition = "mUseRandomWalkDirection == true", EditConditionHides))
	bool mAllowRightDirection = true;

	UPROPERTY(EditAnywhere, DisplayName = "Block Distance", Meta = (ClampMin = 0.1f))
	float mBlockDistance = 100.0f;
};
