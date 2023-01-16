// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallBTServiceBase.h"
#include "BTService_FindLocation.generated.h"

/**
 * 
 */
UCLASS()
class GODFALL_API UBTService_FindLocation : public UGodfallBTServiceBase
{
	GENERATED_BODY()

public:
	UBTService_FindLocation();

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
	class AGodfallEnemyBase* GetPivotEnemy(UBehaviorTreeComponent& OwnerComp) const;
	bool FindLocation(UBehaviorTreeComponent& OwnerComp, FVector& location, FName& directionName) const;
	bool Sweep(class AGodfallEnemyBase* enemy, const FVector& direction, float distance, FName tagName, FName profileName, float& hitDistance) const;

	void SetTargetLocationKey(UBehaviorTreeComponent& OwnerComp);

private:
	UPROPERTY(EditAnywhere, DisplayName = "Pivot Enemy Key")
	FBlackboardKeySelector mPivotEnemyKey;

	UPROPERTY(EditAnywhere, DisplayName = "Target Location Key")
	FBlackboardKeySelector mTargetLocationKey;

	/* 결과가 실패하면 NAME_None
	그 이외에는 Forward, Backward, Left, Right의 결과를 가집니다. */

	UPROPERTY(EditAnywhere, DisplayName = "Direction Key")
	FBlackboardKeySelector mDirectionKey;

	UPROPERTY(EditAnywhere, DisplayName = "Min Distance", Meta = (ClampMin = 0.0f))
	float mMinDistance = 400.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Max Distance", Meta = (ClampMin = 0.0f))
	float mMaxDistance = 800.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Allow Forward")
	bool mAllowForward = false;

	UPROPERTY(EditAnywhere, DisplayName = "Forward Priority", Meta = (EditCondition = "mRandomPriority == false", EditConditionHides))
	uint32 mForwardPrioirty = 3;

	UPROPERTY(EditAnywhere, DisplayName = "Allow Backward")
	bool mAllowBackward = true;

	UPROPERTY(EditAnywhere, DisplayName = "Backward Priority", Meta = (EditCondition = "mRandomPriority == false", EditConditionHides))
	uint32 mBackwardPrioirty = 0;

	UPROPERTY(EditAnywhere, DisplayName = "Allow Left")
	bool mAllowLeft = true;

	UPROPERTY(EditAnywhere, DisplayName = "Left Priority", Meta = (EditCondition = "mRandomPriority == false", EditConditionHides))
	uint32 mLeftPrioirty = 1;

	UPROPERTY(EditAnywhere, DisplayName = "Allow Right")
	bool mAllowRight = true;

	UPROPERTY(EditAnywhere, DisplayName = "Right Priority", Meta = (EditCondition = "mRandomPriority == false", EditConditionHides))
	uint32 mRightPrioirty = 2;

	UPROPERTY(EditAnywhere, DisplayName = "Random Priority")
	bool mRandomPriority = false;
};
