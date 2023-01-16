// Fill out your copyright notice in the Description page of Project Settings.


#include "GodfallEnemyAnimInstanceBase.h"

void UGodfallEnemyAnimInstanceBase::NativeThreadSafeUpdateAnimation(float deltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(deltaTime);

	if (mExecutionMontage && Montage_IsPlaying(mExecutionMontage) && Montage_GetTimeFraction(mExecutionMontage) == 1.0f)
	{
		OnEndExecution.Broadcast();

		if (mExecutionExit)
		{
			Montage_Stop(0.4f, mExecutionMontage);
		}
	}
}

void UGodfallEnemyAnimInstanceBase::OnMontageStartedCallback(UAnimMontage* montage)
{
	Super::OnMontageStartedCallback(montage);

	if (montage == mExecutionMontage)
	{
		OnBeginExecution.Broadcast();
	}
}

void UGodfallEnemyAnimInstanceBase::PlayDashMontage(EEnemyAvoid3Direction direction)
{
	FName section;

	if (direction == EEnemyAvoid3Direction::Bwd)
	{
		int index = FMath::RandRange(0, mDashMontageSection.Dash_Bwd_Names.Num() - 1);
		section = mDashMontageSection.Dash_Bwd_Names[index];
	}
	else if (direction == EEnemyAvoid3Direction::Left)
	{
		int index = FMath::RandRange(0, mDashMontageSection.Dash_Left_Names.Num() - 1);
		section = mDashMontageSection.Dash_Left_Names[index];
	}
	else if (direction == EEnemyAvoid3Direction::Right)
	{
		int index = FMath::RandRange(0, mDashMontageSection.Dash_Right_Names.Num() - 1);
		section = mDashMontageSection.Dash_Right_Names[index];
	}

	if (!ensure(mDashMontage->IsValidSectionName(section))) return;
	Montage_PlayAtSection(mDashMontage, section);
}

void UGodfallEnemyAnimInstanceBase::PlayExecutionMontageSelf(bool canExit)
{
	if (!ensure(mExecutionMontage)) return;

	mExecutionMontage->bEnableAutoBlendOut = false;
	mExecutionExit = canExit;

	StopAllMontages(0.0f);
	//Montage_Play(mExecutionMontage);
	FMontageBlendSettings settings;
	settings.Blend.BlendTime = 0.0f;
	Montage_PlayWithBlendSettings(mExecutionMontage, settings);
}

bool UGodfallEnemyAnimInstanceBase::IsActiveExecutionMontage() const
{
	if (!mExecutionMontage) return false;
	return Montage_IsActive(mExecutionMontage);
}

bool UGodfallEnemyAnimInstanceBase::IgnoreDamage() const
{
	if (Super::IgnoreDamage()) return true;
	if (mExecutionMontage && Montage_IsPlaying(mExecutionMontage)) return true;
	return false;
}
