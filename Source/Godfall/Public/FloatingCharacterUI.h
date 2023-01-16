// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "Blueprint/UserWidget.h"
#include "FloatingCharacterUI.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS()
class GODFALL_API UFloatingCharacterUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	void SetHealthPercent(float percent);
	void SetSteminaPercent(float percent);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess, BindWidget))
	UProgressBar* HealthBar = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess, BindWidget))
	UProgressBar* SteminaBar = nullptr;
};
