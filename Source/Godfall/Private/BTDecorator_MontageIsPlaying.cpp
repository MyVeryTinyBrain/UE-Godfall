// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_MontageIsPlaying.h"
#include <AIController.h>

UBTDecorator_MontageIsPlaying::UBTDecorator_MontageIsPlaying()
{
	NodeName = TEXT("Montage Is Playing");
}

bool UBTDecorator_MontageIsPlaying::CalculateCondition(UBehaviorTreeComponent& OwnerComp) const
{
	UAnimInstance* animInstance = GetAnimInstance(OwnerComp);
	if (!ensure(animInstance)) return false;

	return Montage_IsPlaying(animInstance);
}

UAnimInstance* UBTDecorator_MontageIsPlaying::GetAnimInstance(UBehaviorTreeComponent& OwnerComp) const
{
	ACharacter* character = OwnerComp.GetAIOwner()->GetCharacter();
	UAnimInstance* animInstance = character ? character->GetMesh()->GetAnimInstance() : nullptr;
	return animInstance;
}

bool UBTDecorator_MontageIsPlaying::Montage_IsPlaying(UAnimInstance* animInstance) const
{
	for (auto& query : mQueries)
	{
		if (!query.UseSection)
		{
			if (!query.UseRange)
			{
				if (animInstance->Montage_IsPlaying(query.Montage))
				{
					return true;
				}
			}
			else
			{
				float timeFraction = Montage_GetTimeFraction(animInstance, query.Montage);
				if (timeFraction >= query.MinTimeFraction && timeFraction <= query.MaxTimeFraction)
				{
					return true;
				}
			}
		}
		else
		{
			if (!query.UseRange)
			{
				float timeFraction = Montage_GetTimeFraction(animInstance, query.Montage, query.SectionName);
				if (timeFraction >= 0.0f && timeFraction <= 1.0f)
				{
					return true;
				}
			}
			else
			{
				float timeFraction = Montage_GetTimeFraction(animInstance, query.Montage, query.SectionName);
				if (timeFraction >= query.MinTimeFraction && timeFraction <= query.MaxTimeFraction)
				{
					return true;
				}
			}
		}
	}

	return false;
}

float UBTDecorator_MontageIsPlaying::Montage_GetTimeFraction(UAnimInstance* animInstance, const UAnimMontage* montage) const
{
	if (!animInstance->Montage_IsPlaying(montage))
	{
		return FLT_MIN;
	}

	return animInstance->Montage_GetPosition(montage) / montage->GetPlayLength();
}

float UBTDecorator_MontageIsPlaying::Montage_GetTimeFraction(UAnimInstance* animInstance, const UAnimMontage* montage, const FName& sectionName) const
{
	if (!animInstance->Montage_IsPlaying(montage))
	{
		return FLT_MIN;
	}

	if (!montage->IsValidSectionName(sectionName))
	{
		return FLT_MIN;
	}

	int sectionIndex = montage->GetSectionIndex(sectionName);

	float sectionStart, sectionEnd;
	montage->GetSectionStartAndEndTime(sectionIndex, sectionStart, sectionEnd);

	float sectionDelta = sectionEnd - sectionStart;
	float currentPosition = animInstance->Montage_GetPosition(montage);
	float currentSectionRate = (currentPosition - sectionStart) / sectionDelta;

	return currentSectionRate;
}
