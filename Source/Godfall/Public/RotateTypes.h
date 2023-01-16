#pragma once

#include <EngineMinimal.h>
#include "RotateTypes.generated.h"

UENUM(BlueprintType)
enum class ERotatorOverrideRotator : uint8
{
	NoOverride,
	ConstantRotator,
	LookActorTarget,
	LookLocationTarget,
	LookDirectionTarget,
};

USTRUCT(BlueprintType, Blueprintable)
struct FRotateOverride
{
	GENERATED_USTRUCT_BODY()

	FRotateOverride(){}

	FRotateOverride(bool Override)
	{
		this->Override = Override;
	}

	static const FRotateOverride& NoOverride()
	{
		static FRotateOverride data = FRotateOverride(false);
		return data;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Override = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool OverrideRotate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Rotate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool OverwriteRotate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool OverrideRotateSpeed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ClampMin = 0.0f, EditCondition = "OverrideRotateSpeed == true", EditConditionHides))
	float RotateSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERotatorOverrideRotator OverrideRotator = ERotatorOverrideRotator::NoOverride;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (EditCondition = "OverrideRotator == ERotatorOverrideRotator::ConstantRotator", EditConditionHides))
	FRotator ConstantRotator = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (EditConditionHides, EditCondition = "OverrideRotator == ERotatorOverrideRotator::LookActorTarget || OverrideRotator == ERotatorOverrideRotator::LookLocationTarget || OverrideRotator == ERotatorOverrideRotator::LookDirectionTarget"))
	FString TargetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (EditCondition = "OverrideRotator == ERotatorOverrideRotator::LookDirectionTarget", EditConditionHides))
	FRotator RotateDirection = FRotator::ZeroRotator;
};

