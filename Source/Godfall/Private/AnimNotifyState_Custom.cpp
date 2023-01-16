// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_Custom.h"

UAnimNotifyState_Custom::UAnimNotifyState_Custom()
{
}

FString UAnimNotifyState_Custom::GetNotifyName_Implementation() const
{
	if (!StateName.IsNone())
	{
		return StateName.ToString();
	}

	return UGodfallAnimNotifyStateBase::GetNotifyName_Implementation();
}
