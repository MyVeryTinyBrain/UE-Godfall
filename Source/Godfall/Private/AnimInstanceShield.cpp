// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstanceShield.h"
#include <Animation/AnimNode_StateMachine.h>

bool UAnimInstanceShield::IsOpenToAny() const
{
	if (GetBlendWeight(TEXT("BaseStateMachine"), TEXT("Opening")) > 0.0f) return true;
	if (GetBlendWeight(TEXT("BaseStateMachine"), TEXT("Open")) > 0.0f) return true;
	if (GetBlendWeight(TEXT("BaseStateMachine"), TEXT("Closing")) > 0.0f) return true;
	return false;
}

void UAnimInstanceShield::SetOpen(bool value)
{
	mOpen = value;
}

void UAnimInstanceShield::AnimNotify_OnClosed()
{
	OnShieldClosed.Broadcast();
}

void UAnimInstanceShield::AnimNotify_OnOpen()
{
	OnShieldOpen.Broadcast();
}
