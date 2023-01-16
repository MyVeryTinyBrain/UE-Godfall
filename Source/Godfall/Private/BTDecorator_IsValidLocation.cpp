// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsValidLocation.h"
#include <BehaviorTree/BlackboardComponent.h>

UBTDecorator_IsValidLocation::UBTDecorator_IsValidLocation()
{
	NodeName = TEXT("Is Valid Location");

	mLocationKey.AddVectorFilter(this, NAME_None);
}

bool UBTDecorator_IsValidLocation::CalculateCondition(UBehaviorTreeComponent& OwnerComp) const
{
	return OwnerComp.GetBlackboardComponent()->GetValueAsName(mLocationKey.SelectedKeyName) == NAME_None;
}
