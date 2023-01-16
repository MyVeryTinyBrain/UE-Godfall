// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallBTTaskNodeBase.h"
#include "BTTask_PlayMontage.generated.h"

/**
 * 
 */
UCLASS()
class GODFALL_API UBTTask_PlayMontage : public UGodfallBTTaskNodeBase
{
	GENERATED_BODY()

public:
	UBTTask_PlayMontage();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

public:
	virtual FString GetStaticDescription() const override;

private:
	UAnimInstance* GetAnimInstance(UBehaviorTreeComponent& OwnerComp) const;
	void Montage_SetTimeFraction(UBehaviorTreeComponent& OwnerComp, const UAnimMontage* montage, float timeFraction);
	void Montage_SetTimeFraction_InSection(UBehaviorTreeComponent& OwnerComp, const UAnimMontage* montage, const FName& sectionName, float timeFraction);
	bool Montage_GetTimeFraction(UBehaviorTreeComponent& OwnerComp, const UAnimMontage* montage, float& timeFraction) const;
	bool Montage_GetTimeFraction_InSection(UBehaviorTreeComponent& OwnerComp, const UAnimMontage* montage, const FName& sectionName, float& timeFraction) const;

private:
	UPROPERTY(EditAnywhere, DisplayName = "Montage")
	UAnimMontage* mMontage;

	UPROPERTY(EditAnywhere, DisplayName = "Blend In time")
	float mBlendInTime = -1.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Use start section")
	bool mUseStartSection = false;

	UPROPERTY(EditAnywhere, DisplayName = "Start section name", Meta = (EditCondition = "mUseStartSection == true", EditConditionHides))
	FName mStartSectionName;

	UPROPERTY(EditAnywhere, DisplayName = "Use start time fraction")
	bool mUseStartTimeFraction = false;

	UPROPERTY(EditAnywhere, DisplayName = "Start time fraction", Meta = (ClampMin = 0.0f, ClampMax = 1.0f, EditCondition = "mUseStartTimeFraction == true", EditConditionHides))
	float mStartTimeFraction = 0.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Use end time fraction")
	bool mUseEndTimeFraction = false;

	UPROPERTY(EditAnywhere, DisplayName = "Use end time fraction in section", Meta = (EditCondition = "mUseEndTimeFraction == true", EditConditionHides))
	bool mUseEndTimeFractionOfSection = false;

	UPROPERTY(EditAnywhere, DisplayName = "End time fraction of section name", Meta = (EditCondition = "mUseEndTimeFractionOfSection == true && mUseEndTimeFraction == true", EditConditionHides))
	FName mEndTimeFractionOfSectionName;

	UPROPERTY(EditAnywhere, DisplayName = "End time fraction", Meta = (ClampMin = 0.0f, ClampMax = 1.0f, EditCondition = "mUseEndTimeFraction == true", EditConditionHides))
	float mEndTimeFraction = 1.0f;

	UPROPERTY(EditAnywhere, DisplayName = "BlendOut time", Meta = (ClampMin = 0.0f, EditCondition = "mUseEndTimeFraction == true", EditConditionHides))
	float mBlendOutTime = 0.4f;
};
