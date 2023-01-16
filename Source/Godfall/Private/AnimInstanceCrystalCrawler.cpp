// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstanceCrystalCrawler.h"

UAnimInstanceCrystalCrawler::UAnimInstanceCrystalCrawler()
{
	ConstructorHelpers::FObjectFinder<UAnimMontage> damageMontage(TEXT("/Game/Characters/CrystalCrawler/Montages/AM_CrystalCrawler_Damage.AM_CrystalCrawler_Damage"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> additiveFlinchMontage(TEXT("/Game/Characters/CrystalCrawler/Montages/AM_CrystalCrawler_Additive_Flinch.AM_CrystalCrawler_Additive_Flinch"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> deathMontage(TEXT("/Game/Characters/CrystalCrawler/Montages/AM_CrystalCrawler_Death.AM_CrystalCrawler_Death"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> stunMontage(TEXT("/Game/Characters/CrystalCrawler/Montages/AM_CrystalCrawler_Stun.AM_CrystalCrawler_Stun"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> dashMontage(TEXT("/Game/Characters/CrystalCrawler/Montages/AM_CrystalCrawler_Dash.AM_CrystalCrawler_Dash"));

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
	if (ensure(dashMontage.Succeeded()))
	{
		mDashMontage = dashMontage.Object;
	}
}
