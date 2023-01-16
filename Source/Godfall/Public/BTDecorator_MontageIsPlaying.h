// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallBTDecoratorBase.h"
#include "BTDecorator_MontageIsPlaying.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FBTMontageIsPlayingQuery
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	UAnimMontage* Montage = nullptr;

	UPROPERTY(EditAnywhere)
	bool UseSection = false;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "UseSection == true", EditConditionHides))
	FName SectionName;

	UPROPERTY(EditAnywhere)
	bool UseRange = false;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.0f, ClampMax = 1.0f, EditCondition = "UseRange == true", EditConditionHides))
	float MinTimeFraction = 0.0f;

	UPROPERTY(EditAnywhere, Meta = (ClampMin = 0.1f, ClampMax = 1.0f, EditCondition = "UseRange == true", EditConditionHides))
	float MaxTimeFraction = 1.0f;
};

/**
 * 
 */
UCLASS()
class GODFALL_API UBTDecorator_MontageIsPlaying : public UGodfallBTDecoratorBase
{
	GENERATED_BODY()

public:
	UBTDecorator_MontageIsPlaying();

protected:
	virtual bool CalculateCondition(UBehaviorTreeComponent& OwnerComp) const override;

private:
	UAnimInstance* GetAnimInstance(UBehaviorTreeComponent& OwnerComp) const;
	bool Montage_IsPlaying(UAnimInstance* animInstance) const;
	float Montage_GetTimeFraction(UAnimInstance* animInstance, const UAnimMontage* montage) const;
	float Montage_GetTimeFraction(UAnimInstance* animInstance, const UAnimMontage* montage, const FName& sectionName) const;
	
private:
	UPROPERTY(EditAnywhere, DisplayName = "Queries")
	TArray<FBTMontageIsPlayingQuery> mQueries;
};
