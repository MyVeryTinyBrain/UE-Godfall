// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallAnimNotifyStateBase.h"
#include "AnimNotifyState_Custom.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FCustomNotifyStateVariable
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<bool> Booleans;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> Integers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> Floatings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> Vectors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Names;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (MultiLine = "true"))
	FString Memo;
};

/**
 * 
 */
UCLASS(DisplayName = "Custom")
class GODFALL_API UAnimNotifyState_Custom : public UGodfallAnimNotifyStateBase
{
	GENERATED_BODY()
	
public:
	UAnimNotifyState_Custom();

protected:
	// Overridden from UAnimNotifyState to provide custom notify name.
	FString GetNotifyName_Implementation() const override;

public:
	virtual FName GetFunctionName() const override { return TEXT("AnimNotifyState_Custom"); };
	const FName& GetNotifyStateName() const { return StateName; }
	const FCustomNotifyStateVariable& GetCustomVariables() const { return CustomVariables; }

private:
	UPROPERTY(EditAnywhere, DisplayName = "State Name", Meta = (AllowPrivateAccess))
	FName StateName = NAME_None;

	UPROPERTY(EditAnywhere, DisplayName = "Custom Variables", Meta = (AllowPrivateAccess))
	FCustomNotifyStateVariable CustomVariables;
};
