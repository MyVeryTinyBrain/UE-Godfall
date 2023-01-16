// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstanceEarthAberrant.h"

UAnimInstanceEarthAberrant::UAnimInstanceEarthAberrant()
{
	ConstructorHelpers::FObjectFinder<UAnimMontage> damageMontage(TEXT("/Game/Characters/GrievesMinion_Armored/Montages/AM_Earth_Aberrant_Damage.AM_Earth_Aberrant_Damage"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> additiveFlinchMontage(TEXT("/Game/Characters/GrievesMinion_Armored/Montages/AM_Earth_Aberrant_Additive_Flinch.AM_Earth_Aberrant_Additive_Flinch"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> deathMontage(TEXT("/Game/Characters/GrievesMinion_Armored/Montages/AM_Earth_Aberrant_Death.AM_Earth_Aberrant_Death"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> stunMontage(TEXT("/Game/Characters/GrievesMinion_Armored/Montages/AM_Earth_Aberrant_Stun.AM_Earth_Aberrant_Stun"));	
	ConstructorHelpers::FObjectFinder<UAnimMontage> executionMontage(TEXT("/Game/Characters/GrievesMinion_Armored/Montages/AM_Earth_Aberrant_Execution.AM_Earth_Aberrant_Execution"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> dashMontage(TEXT("/Game/Characters/GrievesMinion_Armored/Montages/AM_Earth_Aberrant_Dash.AM_Earth_Aberrant_Dash"));
	
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

	mDamageMontageSection.Stagger_Left_Names = { TEXT("Stagger_Left_1"), TEXT("Stagger_Left_2"), TEXT("Stagger_Left_3") };
	mDamageMontageSection.Stagger_Right_Names = { TEXT("Stagger_Right_1"), TEXT("Stagger_Right_2"), TEXT("Stagger_Right_3") };
}

void UAnimInstanceEarthAberrant::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}
