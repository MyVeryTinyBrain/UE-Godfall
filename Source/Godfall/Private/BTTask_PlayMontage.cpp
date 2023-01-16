// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PlayMontage.h"
#include <AIController.h>
UBTTask_PlayMontage::UBTTask_PlayMontage()
{
	NodeName = TEXT("Play Montage");
}

EBTNodeResult::Type UBTTask_PlayMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(!ensure(mMontage)) return EBTNodeResult::Failed;

	UAnimInstance* animInstance = GetAnimInstance(OwnerComp);
	if (!ensure(animInstance)) return EBTNodeResult::Failed;

	if (mBlendInTime < 0.0f)
	{
		animInstance->Montage_Play(mMontage);
	}
	else
	{
		FMontageBlendSettings blendInSettings(mBlendInTime);
		animInstance->Montage_PlayWithBlendSettings(mMontage, blendInSettings);
	}

	if (mUseStartSection)
	{
		if (ensure(mMontage->IsValidSectionName(mStartSectionName)))
		{
			animInstance->Montage_JumpToSection(mStartSectionName, mMontage);
		}
	}

	if (mUseStartTimeFraction)
	{
		if (mUseStartSection)
		{
			Montage_SetTimeFraction_InSection(OwnerComp, mMontage, mStartSectionName, mStartTimeFraction);
		}
		else
		{
			Montage_SetTimeFraction(OwnerComp, mMontage, mStartTimeFraction);
		}
	}

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_PlayMontage::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Aborted;
}

void UBTTask_PlayMontage::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UAnimInstance* animInstance = GetAnimInstance(OwnerComp);
	if (!ensure(animInstance))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	if (!mUseEndTimeFraction)
	{
		if (!animInstance->Montage_IsPlaying(mMontage))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}
	else
	{
		if (!mUseEndTimeFractionOfSection)
		{
			float playRate;
			bool succeeded = Montage_GetTimeFraction(OwnerComp, mMontage, playRate);

			if (!succeeded)
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
				return;
			}
			else if (playRate >= mEndTimeFraction)
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return;
			}
		}
		else
		{
			float playRate;
			bool succeeded = Montage_GetTimeFraction_InSection(OwnerComp, mMontage, mEndTimeFractionOfSectionName, playRate);

			if (!succeeded)
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
				return;
			}
			else if (playRate >= mEndTimeFraction)
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return;
			}
		}
	}
}

void UBTTask_PlayMontage::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (!ensure(mMontage)) return;

	UAnimInstance* animInstance = GetAnimInstance(OwnerComp);
	if (!animInstance) return;

	if (animInstance->Montage_IsPlaying(mMontage))
	{
		animInstance->Montage_Stop(mBlendOutTime, mMontage);
	}	
}

FString UBTTask_PlayMontage::GetStaticDescription() const
{
	return FString::Printf(TEXT("Play: %s"), (mMontage ? *mMontage->GetName() : TEXT("None")));
}

UAnimInstance* UBTTask_PlayMontage::GetAnimInstance(UBehaviorTreeComponent& OwnerComp) const
{
	ACharacter* character = OwnerComp.GetAIOwner()->GetCharacter();
	UAnimInstance* animInstance = character ? character->GetMesh()->GetAnimInstance() : nullptr;
	return animInstance;
}

void UBTTask_PlayMontage::Montage_SetTimeFraction(UBehaviorTreeComponent& OwnerComp, const UAnimMontage* montage, float timeFraction)
{
	if (!ensure(montage)) return;

	UAnimInstance* animInstance = GetAnimInstance(OwnerComp);
	if (!ensure(animInstance)) return;

	if (!animInstance->Montage_IsPlaying(montage)) return;

	float newPosition = montage->GetPlayLength() * timeFraction;
	animInstance->Montage_SetPosition(montage, newPosition);
}

void UBTTask_PlayMontage::Montage_SetTimeFraction_InSection(UBehaviorTreeComponent& OwnerComp, const UAnimMontage* montage, const FName& sectionName, float timeFraction)
{
	if (!ensure(montage)) return;

	if (!ensure(montage->IsValidSectionName(sectionName))) return;

	UAnimInstance* animInstance = GetAnimInstance(OwnerComp);
	if (!ensure(animInstance)) return;

	if (!animInstance->Montage_IsPlaying(montage)) return;

	FName currentSectionName = animInstance->Montage_GetCurrentSection(montage);
	int currentSectionIndex = montage->GetSectionIndex(currentSectionName);

	float sectionStart, sectionEnd;
	montage->GetSectionStartAndEndTime(currentSectionIndex, sectionStart, sectionEnd);

	float sectionDelta = sectionEnd - sectionStart;
	float newPosition = sectionStart + sectionDelta * timeFraction;
	animInstance->Montage_SetPosition(montage, newPosition);
}

bool UBTTask_PlayMontage::Montage_GetTimeFraction(UBehaviorTreeComponent& OwnerComp, const UAnimMontage* montage, float& timeFraction) const
{
	timeFraction = 0.0f;

	if (!ensure(montage)) return false;

	UAnimInstance* animInstance = GetAnimInstance(OwnerComp);
	if (!ensure(animInstance)) return false;

	if (!animInstance->Montage_IsPlaying(montage))
	{
		timeFraction = 1.0f;
		return true;
	}

	timeFraction = animInstance->Montage_GetPosition(montage) / montage->GetPlayLength();
	return true;
}

bool UBTTask_PlayMontage::Montage_GetTimeFraction_InSection(UBehaviorTreeComponent& OwnerComp, const UAnimMontage* montage, const FName& sectionName, float& timeFraction) const
{
	timeFraction = 0.0f;

	if (!ensure(montage)) return false;

	if (!ensure(montage->IsValidSectionName(sectionName))) return false;

	UAnimInstance* animInstance = GetAnimInstance(OwnerComp);
	if (!ensure(animInstance)) return false;
	
	if (!animInstance->Montage_IsPlaying(montage)) 
	{
		timeFraction = 1.0f;
		return true;
	}

	int sectionIndex = montage->GetSectionIndex(sectionName);

	float sectionStart, sectionEnd;
	montage->GetSectionStartAndEndTime(sectionIndex, sectionStart, sectionEnd);

	float sectionDelta = sectionEnd - sectionStart;
	float currentPosition = animInstance->Montage_GetPosition(montage);
	float currentSectionRate = (currentPosition - sectionStart) / sectionDelta;

	timeFraction = currentSectionRate;
	return true;
}
