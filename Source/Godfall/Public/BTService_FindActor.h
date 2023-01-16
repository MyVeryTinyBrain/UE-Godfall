// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallBTServiceBase.h"
#include "BTService_FindActor.generated.h"

UENUM(BlueprintType)
enum class EServiceFindActorType : uint8
{
	Player,
};

/**
 * 
 */
UCLASS()
class GODFALL_API UBTService_FindActor : public UGodfallBTServiceBase
{
	GENERATED_BODY()
	
public:
	UBTService_FindActor();

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
	void FindActor(UBehaviorTreeComponent& OwnerComp);

	bool IsFindableActor(const AActor* ownerActor, const AActor* otherActor) const;

private:
	UPROPERTY(EditAnywhere, DisplayName = "Find Type")
	EServiceFindActorType mFindType = EServiceFindActorType::Player;

	UPROPERTY(EditAnywhere, DisplayName = "Target Actor Key")
	FBlackboardKeySelector mTargetActorKey;

	UPROPERTY(EditAnywhere, DisplayName = "Use Radius")
	bool mUseRadius = false;

	UPROPERTY(EditAnywhere, DisplayName = "Radius", Meta = (ClampMin = 0.0f))
	float mRadius = 1000.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Use Half Angle")
	bool mUseHalfAngle = false;

	UPROPERTY(EditAnywhere, DisplayName = "Half Angle", Meta = (ClampMin = 0.0f, ClampMax = 180.0f))
	float mHalfAngle = 180.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Use Raycast")
	bool mUseRaycast = true;
};
