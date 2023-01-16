// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstanceSolaris.h"

UAnimInstanceSolaris::UAnimInstanceSolaris()
{
	ConstructorHelpers::FObjectFinder<UAnimMontage> damageMontage(TEXT("/Game/Characters/Solaris/Montages/AM_Solaris_Damage.AM_Solaris_Damage"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> additiveFlinchMontage(TEXT("/Game/Characters/Solaris/Montages/AM_Solaris_Additive_Flinch.AM_Solaris_Additive_Flinch"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> deathMontage(TEXT("/Game/Characters/Solaris/Montages/AM_Solaris_Death.AM_Solaris_Death"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> stunMontage(TEXT("/Game/Characters/Solaris/Montages/AM_Solaris_Stun.AM_Solaris_Stun"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> executionMontage(TEXT("/Game/Characters/Solaris/Montages/AM_Solaris_Execution.AM_Solaris_Execution"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> dashMontage(TEXT("/Game/Characters/Solaris/Montages/AM_Solaris_Dash.AM_Solaris_Dash"));

	if (ensure(damageMontage.Succeeded()))
	{
		mDamageMontage = damageMontage.Object;
	}
	if (ensure(additiveFlinchMontage.Succeeded()))
	{
		mAdditiveFlinchMontage = additiveFlinchMontage.Object;
	}
	if (ensure(deathMontage.Succeeded()))
	{
		mDeathMontage = deathMontage.Object;
	}
	if (ensure(stunMontage.Succeeded()))
	{
		mStunMontage = stunMontage.Object;
	}
	if (ensure(executionMontage.Succeeded()))
	{
		mExecutionMontage = executionMontage.Object;
	}
	if (ensure(dashMontage.Succeeded()))
	{
		mDashMontage = dashMontage.Object;
	}

	mDamageMontageSection.Stagger_Left_Names = {  TEXT("Flinch_Left") };
	mDamageMontageSection.Stagger_Right_Names = { TEXT("Flinch_Right") };
}

void UAnimInstanceSolaris::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}
