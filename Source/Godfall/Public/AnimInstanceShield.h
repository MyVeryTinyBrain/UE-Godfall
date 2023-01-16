// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallAnimInstanceBase.h"
#include "AnimInstanceShield.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldClosed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldOpen);

/**
 * 
 */
UCLASS()
class GODFALL_API UAnimInstanceShield : public UGodfallAnimInstanceBase
{
	GENERATED_BODY()

public:
	bool IsOpenToAny() const;
	void SetOpen(bool value);

private:
	UFUNCTION()
	void AnimNotify_OnClosed();

	UFUNCTION()
	void AnimNotify_OnOpen();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Open")
	bool mOpen = false;

public:
	FOnShieldClosed OnShieldClosed;
	FOnShieldOpen OnShieldOpen;
};
