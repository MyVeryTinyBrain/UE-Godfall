// Fill out your copyright notice in the Description page of Project Settings.


#include "GodfallFollowingComponent.h"
#include "GodfallEnemyBase.h"

UGodfallFollowingComponent::UGodfallFollowingComponent()
{
	//bCheckMovementAngle = false;
	//bFinalPathPart = false;
	//bRotateToVelocity = false;
}

void UGodfallFollowingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
}

void UGodfallFollowingComponent::FollowPathSegment(float DeltaTime)
{
}

void UGodfallFollowingComponent::UpdatePathSegment()
{
}
