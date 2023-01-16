// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_WalkTo.h"
#include "GodfallEnemyBase.h"
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Vector.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Object.h>
#include "EnemyMoveComponent.h"
#include "GodfallUtil.h"

UBTTaskNode_WalkTo::UBTTaskNode_WalkTo()
{
	NodeName = TEXT("Walk To");

	mTargetKey.AddVectorFilter(this, NAME_None);
	mTargetKey.AddObjectFilter(this, NAME_None, AActor::StaticClass());
}

EBTNodeResult::Type UBTTaskNode_WalkTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AGodfallEnemyBase* enemy = GetEnemy(OwnerComp);
	if (!ensure(enemy)) return EBTNodeResult::Failed;

	UEnemyMoveComponent* enemyMoveComp = enemy->GetEnemyMoveComponent();
	if (!ensure(enemyMoveComp)) return EBTNodeResult::Failed;

	TNodeInstanceMemory* DecoratorMemory = CastInstanceNodeMemory<TNodeInstanceMemory>(NodeMemory);
	DecoratorMemory->InitialDirection = mWalkDirection;
	if (mUseRandomWalkDirection)
	{
		TArray<E4Direction> directions;
		if (mAllowForwardDirection) directions.Add(E4Direction::Forward);
		if (mAllowBackwardDirection) directions.Add(E4Direction::Backward);
		if (mAllowLeftDirection) directions.Add(E4Direction::Left);
		if (mAllowRightDirection) directions.Add(E4Direction::Right);

		if (directions.Num() > 0)
		{
			GodfallUtil::Sort::Mix(directions, directions.Num());
			DecoratorMemory->InitialDirection = directions[0];
		}
	}

	FVector walkDirection;
	if (!GetWalkDirection(OwnerComp, DecoratorMemory->InitialDirection, walkDirection))
	{
		return EBTNodeResult::Failed;
	}
	float tempDistance = mWalkDistance;
	enemyMoveComp->WalkToLocation(enemy->GetActorLocation() + walkDirection * tempDistance, DecoratorMemory->InitialDirection);
	enemyMoveComp->mStopIfPartialPath = true;
	enemyMoveComp->mLookOnChangedMoveState = false;
	enemyMoveComp->mWalkDirection = DecoratorMemory->InitialDirection;
	enemyMoveComp->mUseWait = true;
	enemyMoveComp->mLookOnWait = true;
	enemyMoveComp->mBlockDistance = mBlockDistance;
	enemyMoveComp->mStopRadius = mStopRadius;
	enemyMoveComp->mUseAvoid = false;
	enemyMoveComp->mAvoidDistance = 0.0f;
	enemyMoveComp->mAvoidCollisionIgnore = GetTargetActor(OwnerComp.GetBlackboardComponent());

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTaskNode_WalkTo::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}

void UBTTaskNode_WalkTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

	TNodeInstanceMemory* DecoratorMemory = CastInstanceNodeMemory<TNodeInstanceMemory>(NodeMemory);
	FVector walkDirection;
	if (!GetWalkDirection(OwnerComp, DecoratorMemory->InitialDirection, walkDirection))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	float tempDistance = mWalkDistance;
	enemyMoveComp->UptateTargetLocation(enemy->GetActorLocation() + walkDirection * tempDistance);

	EEnemyMoveComponentResult result = enemyMoveComp->GetResult();
	if (result != EEnemyMoveComponentResult::InProgress)
	{
		switch (result)
		{
			case EEnemyMoveComponentResult::Succeeded:
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				break;
			case EEnemyMoveComponentResult::Failed:
				// 경로에 갈 수 없어도 성공으로 치고 종료합니다.
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				break;
		}
		return;
	}

	switch (enemyMoveComp->GetMoveState())
	{
		case EEnemyMoveComponentState::Wait:
		case EEnemyMoveComponentState::Avoid:
		case EEnemyMoveComponentState::Stop:
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
		break;
	}

	if (mUseStopRadiusWithNearTarget && enemyMoveComp->IsNearWith(GetTargetActor(OwnerComp.GetBlackboardComponent()), mStopRadiusWithNearTarget))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	if (enemyMoveComp->GetMoveDistanceAccumulation() > mWalkDistance)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
}

void UBTTaskNode_WalkTo::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	AGodfallEnemyBase* enemy = GetEnemy(OwnerComp);
	if (!enemy) return;

	UEnemyMoveComponent* enemyMoveComp = enemy->GetEnemyMoveComponent();
	if (!ensure(enemyMoveComp)) return;

	enemyMoveComp->Stop();
}

bool UBTTaskNode_WalkTo::GetTargetLocation(const UBlackboardComponent* blackboardComp, FVector& location) const
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

AGodfallEnemyBase* UBTTaskNode_WalkTo::GetEnemy(UBehaviorTreeComponent& OwnerComp) const
{
	return Cast<AGodfallEnemyBase>(OwnerComp.GetAIOwner()->GetPawn());
}

const AActor* UBTTaskNode_WalkTo::GetTargetActor(const UBlackboardComponent* blackboardComp) const
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

bool UBTTaskNode_WalkTo::IsNearWithTarget(UBehaviorTreeComponent& OwnerComp) const
{
	AGodfallEnemyBase* enemy = GetEnemy(OwnerComp);
	if (!ensure(enemy)) return false;

	UEnemyMoveComponent* enemyMoveComp = enemy->GetEnemyMoveComponent();
	if (!ensure(enemyMoveComp)) return false;

	UBlackboardComponent* blackboardComp = OwnerComp.GetBlackboardComponent();
	if (!ensure(blackboardComp)) return false;

	FBlackboard::FKey keyID = blackboardComp->GetKeyID(mTargetKey.SelectedKeyName);
	TSubclassOf<UBlackboardKeyType> keyType = blackboardComp->GetKeyType(keyID);

	if (keyType == UBlackboardKeyType_Vector::StaticClass())
	{
		FVector location = blackboardComp->GetValueAsVector(mTargetKey.SelectedKeyName);
		if (location == FAISystem::InvalidLocation) return false;
		else return enemyMoveComp->IsNearWith(location, mStopRadius);
	}
	else if (keyType == UBlackboardKeyType_Object::StaticClass())
	{
		AActor* actor = Cast<AActor>(blackboardComp->GetValueAsObject(mTargetKey.SelectedKeyName));
		if (!ensure(actor)) return false;
		else return enemyMoveComp->IsNearWith(actor, mStopRadius);
	}

	return false;
}

bool UBTTaskNode_WalkTo::GetWalkDirection(UBehaviorTreeComponent& OwnerComp, E4Direction direction, FVector& vector) const
{
	AGodfallEnemyBase* enemy = GetEnemy(OwnerComp);
	if (!ensure(enemy)) return false;

	UEnemyMoveComponent* enemyMoveComp = enemy->GetEnemyMoveComponent();
	if (!ensure(enemyMoveComp)) return false;

	UBlackboardComponent* blackboardComp = OwnerComp.GetBlackboardComponent();
	if (!ensure(blackboardComp)) return false;

	FVector targetLocation;
	if (!(GetTargetLocation(OwnerComp.GetBlackboardComponent(), targetLocation)))
	{
		return false;
	}
	FVector ownerToTarget = targetLocation - enemy->GetActorLocation();
	ownerToTarget = FVector::VectorPlaneProject(ownerToTarget, FVector::UpVector).GetSafeNormal();

	FVector vWalkDirection;
	switch (direction)
	{
		case E4Direction::Backward:
			vWalkDirection = FRotator(0.0f, 180.0f, 0.0f).RotateVector(ownerToTarget);
			break;
		case E4Direction::Left:
			vWalkDirection = FRotator(0.0f, -90.0f, 0.0f).RotateVector(ownerToTarget);
			break;
		case E4Direction::Right:
			vWalkDirection = FRotator(0.0f, 90.0f, 0.0f).RotateVector(ownerToTarget);
			break;
		default:
			vWalkDirection = ownerToTarget;
			break;
	}

	vector = vWalkDirection;
	return true;
}
