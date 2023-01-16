// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_FindLocation.h"
#include <BehaviorTree/Blackboard/BlackboardKeyType_Object.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Vector.h>
#include "GodfallEnemyBase.h"
#include <BehaviorTree/BlackboardComponent.h>
#include "GodfallUtil.h"

UBTService_FindLocation::UBTService_FindLocation()
{
	NodeName = TEXT("Find Location");

	mEditExecuteFlags = true;
	mExecuteOnSearchStart = true;
	mExecuteOnBecomeRelevant = false;
	mExecuteOnCeaseRelevant = false;
	mExecuteOnTick = false;

	mPivotEnemyKey.AddObjectFilter(this, NAME_None, AGodfallEnemyBase::StaticClass());
	mTargetLocationKey.AddVectorFilter(this, NAME_None);
	mDirectionKey.AddNameFilter(this, NAME_None);
}

void UBTService_FindLocation::OnSearchStartEvent(FBehaviorTreeSearchData& SearchData)
{
	SetTargetLocationKey(SearchData.OwnerComp);
}

void UBTService_FindLocation::OnBecomeRelevantEvent(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	SetTargetLocationKey(OwnerComp);
}

void UBTService_FindLocation::OnCeaseRelevantEvent(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	SetTargetLocationKey(OwnerComp);
}

void UBTService_FindLocation::TickNodeEvent(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	SetTargetLocationKey(OwnerComp);
}

AGodfallEnemyBase* UBTService_FindLocation::GetPivotEnemy(UBehaviorTreeComponent& OwnerComp) const
{
	UBlackboardComponent* blackboardComp = OwnerComp.GetBlackboardComponent();
	
	if (!ensure(blackboardComp)) return nullptr;

	FBlackboard::FKey keyID = blackboardComp->GetKeyID(mPivotEnemyKey.SelectedKeyName);
	TSubclassOf<UBlackboardKeyType> keyType = blackboardComp->GetKeyType(keyID);

	return Cast<AGodfallEnemyBase>(blackboardComp->GetValueAsObject(mPivotEnemyKey.SelectedKeyName));
}

bool UBTService_FindLocation::FindLocation(UBehaviorTreeComponent& OwnerComp, FVector& location, FName& directionName) const
{
	AGodfallEnemyBase* enemy = GetPivotEnemy(OwnerComp);
	if (!enemy) return false;

	struct FDirectionPriorty
	{
		FVector Direction;
		uint32 Priority;
		FName DirectionName;
		FDirectionPriorty(const FVector& Direction, uint32 Priority, const FName& DirectionName)
		{
			this->Direction = Direction;
			this->Priority = Priority;
			this->DirectionName = DirectionName;
		}
	};
	TArray<FDirectionPriorty> directions;
	if (mAllowBackward) directions.Add(FDirectionPriorty(-enemy->GetActorForwardVector(), mBackwardPrioirty, TEXT("Backward")));
	if (mAllowForward) directions.Add(FDirectionPriorty(enemy->GetActorForwardVector(), mForwardPrioirty, TEXT("Forward")));
	if (mAllowLeft) directions.Add(FDirectionPriorty(-enemy->GetActorRightVector(), mLeftPrioirty, TEXT("Left")));
	if (mAllowRight) directions.Add(FDirectionPriorty(enemy->GetActorRightVector(), mRightPrioirty, TEXT("Right")));
	if (mRandomPriority)
	{
		GodfallUtil::Sort::Mix(directions, directions.Num());
	}
	else
	{
		directions.Sort([](const FDirectionPriorty& lhs, const FDirectionPriorty& rhs)
		{
			return lhs.Priority < rhs.Priority;
		});
	}

	for (auto& direction : directions)
	{
		FVector xyDirection = FVector::VectorPlaneProject(direction.Direction, FVector::UpVector);
		xyDirection.Normalize();
		float hitDistance;
		bool sweepResult = Sweep(enemy, xyDirection, mMaxDistance, NAME_None, GodfallPresets::SweepPawn, hitDistance);

		if (sweepResult && hitDistance >= mMinDistance && hitDistance <= mMaxDistance)
		{
			location = enemy->GetActorLocation() + xyDirection * hitDistance;
			directionName = direction.DirectionName;
			return true;
		}
		else if (!sweepResult)
		{
			location = enemy->GetActorLocation() + xyDirection * mMaxDistance;
			directionName = direction.DirectionName;
			return true;
		}
	}

	return false;
}

bool UBTService_FindLocation::Sweep(AGodfallEnemyBase* enemy, const FVector& direction, float distance, FName tagName, FName profileName, float& hitDistance) const
{
	if (!enemy) return false;

	UCapsuleComponent* capsuleComp = enemy->GetCapsuleComponent();
	FCollisionShape capsuleShape = capsuleComp->GetCollisionShape();
	FVector capsuleLocation = capsuleComp->GetComponentLocation();
	FRotator capsuleRotation = capsuleComp->GetComponentRotation();
	FCollisionQueryParams params(tagName, false, enemy);

	FHitResult hit;
	bool sweep = GetWorld()->SweepSingleByProfile(
		hit,
		capsuleLocation,
		capsuleLocation + direction * distance,
		capsuleRotation.Quaternion(),
		profileName,
		capsuleShape,
		params);

	hitDistance = hit.Distance;

	return sweep;
}

void UBTService_FindLocation::SetTargetLocationKey(UBehaviorTreeComponent& OwnerComp)
{
	FVector location;
	FName directionName;
	if (FindLocation(OwnerComp, location, directionName))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(mTargetLocationKey.SelectedKeyName, location);
		OwnerComp.GetBlackboardComponent()->SetValueAsName(mDirectionKey.SelectedKeyName, directionName);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(mTargetLocationKey.SelectedKeyName, FAISystem::InvalidLocation);
		OwnerComp.GetBlackboardComponent()->SetValueAsName(mDirectionKey.SelectedKeyName, directionName);
	}
}
