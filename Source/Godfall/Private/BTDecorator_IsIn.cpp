// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsIn.h"
#include "GodfallUtil.h"
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Vector.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Object.h>
#include <AIController.h>

UBTDecorator_IsIn::UBTDecorator_IsIn()
{
	NodeName = TEXT("Is In");

	mPivotKey.AddVectorFilter(this, NAME_None);
	mPivotKey.AddObjectFilter(this, NAME_None, AActor::StaticClass());

	mTargetKey.AddVectorFilter(this, NAME_None);
	mTargetKey.AddObjectFilter(this, NAME_None, AActor::StaticClass());
}

bool UBTDecorator_IsIn::CalculateCondition(UBehaviorTreeComponent& OwnerComp) const
{
	FVector pivotLocation;
	if (!ensure(GetPivotLocation(OwnerComp.GetBlackboardComponent(), pivotLocation))) return false;

	FVector targetLocation;
	if (!(GetTargetLocation(OwnerComp.GetBlackboardComponent(), targetLocation))) return false;

	if (!mIsCone)
	{
		return (FVector::Distance(pivotLocation, targetLocation) <= mRadius);
	}
	else
	{
		FVector coneDirection;
		switch (mConeDirectionMode)
		{
			case EIsInConeDirectionMode::ToTarget:
				coneDirection = (targetLocation - pivotLocation).GetSafeNormal();
				break;
			case EIsInConeDirectionMode::ActorForward:
				coneDirection = OwnerComp.GetAIOwner()->GetPawn()->GetActorForwardVector();
				break;
		}

		GodfallUtil::FConeShape cone(pivotLocation, coneDirection, mRadius, mHalfAngle);
		return cone.Contains(targetLocation);
	}
}

bool UBTDecorator_IsIn::GetPivotLocation(const UBlackboardComponent* blackboardComp, FVector& location) const
{
	if (!ensure(blackboardComp)) return false;

	FBlackboard::FKey keyID = blackboardComp->GetKeyID(mPivotKey.SelectedKeyName);
	TSubclassOf<UBlackboardKeyType> keyType = blackboardComp->GetKeyType(keyID);

	if (keyType == UBlackboardKeyType_Vector::StaticClass())
	{
		location = blackboardComp->GetValueAsVector(mPivotKey.SelectedKeyName);
		if (location == FAISystem::InvalidLocation) return false;
		else return true;
	}
	else if (keyType == UBlackboardKeyType_Object::StaticClass())
	{
		AActor* actor = Cast<AActor>(blackboardComp->GetValueAsObject(mPivotKey.SelectedKeyName));
		if (!ensure(actor)) return false;

		location = actor->GetActorLocation();
		return true;
	}

	return false;
}

bool UBTDecorator_IsIn::GetTargetLocation(const UBlackboardComponent* blackboardComp, FVector& location) const
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