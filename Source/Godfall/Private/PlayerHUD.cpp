// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	HealthBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("HealthBar")));
	SteminaBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("SteminaBar")));

	if (!ensure(HealthBar) &&
		!ensure(SteminaBar))
	{
		return;
	}
}

void UPlayerHUD::SetHealthPercent(float percent)
{
	if (ensure(HealthBar))
	{
		HealthBar->SetPercent(FMath::Clamp(percent, 0.0f, 1.0f));
	}
}

void UPlayerHUD::SetSteminaPercent(float percent)
{
	if (ensure(SteminaBar))
	{
		SteminaBar->SetPercent(FMath::Clamp(percent, 0.0f, 1.0f));
	}
}
