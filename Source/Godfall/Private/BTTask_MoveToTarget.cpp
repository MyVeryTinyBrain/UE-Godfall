// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToTarget.h"
#include "GodfallEnemyBase.h"
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Vector.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Object.h>
#include "EnemyMoveComponent.h"

UBTTask_MoveToTarget::UBTTask_MoveToTarget()
{
	NodeName = TEXT("Move To Target");

	mTargetKey.AddVectorFilter(this, NAME_None);
	mTargetKey.AddObjectFilter(this, NAME_None, AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_MoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AGodfallEnemyBase* enemy = GetEnemy(OwnerComp);
	if (!ensure(enemy)) return EBTNodeResult::Failed;

	UEnemyMoveComponent* enemyMoveComp = enemy->GetEnemyMoveComponent();
	if(!ensure(enemyMoveComp)) return EBTNodeResult::Failed;

	FVector targetLocation;
	if (!(GetTargetLocation(OwnerComp.GetBlackboardComponent(), targetLocation))) return EBTNodeResult::Failed;

	switch (mMoveType)
	{
		case EEnemyMoveType::Walk:
		{
			enemyMoveComp->WalkToLocation(targetLocation, mWalkDirection);
		}
		break;
		case EEnemyMoveType::Run:
		{
			enemyMoveComp->RunToLocation(targetLocation);
		}
		break;
	}

	enemyMoveComp->mStopIfPartialPath = mStopIfPartialPath;
	enemyMoveComp->mLookOnChangedMoveState = mLookOnChangedMoveState;
	enemyMoveComp->mWalkDirection = mWalkDirection;
	enemyMoveComp->mUseWait = mUseWait;
	enemyMoveComp->mLookOnWait = mLookOnWait;
	enemyMoveComp->mBlockDistance = mBlockDistance;
	enemyMoveComp->mStopRadius = mStopRadius;
	enemyMoveComp->mUseAvoid = mUseAvoid;
	enemyMoveComp->mAvoidDistance = mAvoidDistance;
	enemyMoveComp->mAvoidCollisionIgnore = GetTargetActor(OwnerComp.GetBlackboardComponent());

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_MoveToTarget::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}

void UBTTask_MoveToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AGodfallEnemyBase* enemy = GetEnemy(OwnerComp);
	if (!ensure(enemy))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UEnemyMoveComponent* enemyMoveComp = enemy->GetEnemyMoveComponent();
	if (!ensure(enemyMoveComp))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (enemyMoveComp->IsNearWithTarget())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	EEnemyMoveComponentResult result = enemyMoveComp->GetResult();
	if (result != EEnemyMoveComponentResult::InProgress)
	{
		switch (result)
		{
			case EEnemyMoveComponentResult::Succeeded:
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				break;
			case EEnemyMoveComponentResult::Failed:
				FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
				break;
		}
		return;
	}

	FVector targetLocation;
	if (!ensure(GetTargetLocation(OwnerComp.GetBlackboardComponent(), targetLocation)))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	else
	{
		enemyMoveComp->UptateTargetLocation(targetLocation);
	}
}

void UBTTask_MoveToTarget::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	AGodfallEnemyBase* enemy = GetEnemy(OwnerComp);
	if (!enemy) return;

	UEnemyMoveComponent* enemyMoveComp = enemy->GetEnemyMoveComponent();
	if (!ensure(enemyMoveComp)) return;

	enemyMoveComp->Stop();
}

bool UBTTask_MoveToTarget::GetTargetLocation(const UBlackboardComponent* blackboardComp, FVector& location) const
{
	if (!ensure(blackboardComp)) return false;

	FBlackboard::FKey keyID = blackboardComp->GetKeyID(mTargetKey.SelectedKeyName);
	TSubclassOf<UBlackboardKeyType> keyType = blackboardComp->GetKeyType(keyID);

	if (keyType == UBlackboardKeyType_Vector::StaticClass())
	{
		location = blackboardComp->GetValueAsVector(mTargetKey.SelectedKeyName);
		if (location == FAISystem::InvalidLocation) return false;
		else return true;
	}
	else if (keyType == UBlackboardKeyType_Object::StaticClass())
	{
		AActor* actor = Cast<AActor>(blackboardComp->GetValueAsObject(mTargetKey.SelectedKeyName));
		if (!ensure(actor)) return false;

		location = actor->GetActorLocation();
		return true;
	}

	return false;
}

AGodfallEnemyBase* UBTTask_MoveToTarget::GetEnemy(UBehaviorTreeComponent& OwnerComp) const
{
	return Cast<AGodfallEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
}

const AActor* UBTTask_MoveToTarget::GetTargetActor(const UBlackboardComponent* blackboardComp) const
{
	if (!ensure(blackboardComp)) return nullptr;

	FBlackboard::FKey keyID = blackboardComp->GetKeyID(mTargetKey.SelectedKeyName);
	TSubclassOf<UBlackboardKeyType> keyType = blackboardComp->GetKeyType(keyID);

	if (keyType == UBlackboardKeyType_Vector::StaticClass())
	{
		return nullptr;
	}
	else if (keyType == UBlackboardKeyType_Object::StaticClass())
	{
		AActor* actor = Cast<AActor>(blackboardComp->GetValueAsObject(mTargetKey.SelectedKeyName));
		return actor;
	}

	return nullptr;
}

