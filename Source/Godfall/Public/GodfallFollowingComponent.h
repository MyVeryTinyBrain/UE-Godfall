// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "GodfallFollowingComponent.generated.h"

/**
 * 
 */
UCLASS()
class GODFALL_API UGodfallFollowingComponent : public UCrowdFollowingComponent
{
	GENERATED_BODY()
	
public:
	UGodfallFollowingComponent();

protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void FollowPathSegment(float DeltaTime) override;
	virtual void UpdatePathSegment() override;
};
