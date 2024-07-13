// Fill out your copyright notice in the Description page of Project Settings.


#include "GodfallCharacterAnimInstanceBase.h"
#include "AnimNotifyState_Damagable.h"
#include "AnimNotifyState_Rotate.h"

void UGodfallCharacterAnimInstanceBase::NativeThreadSafeUpdateAnimation(float deltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(deltaTime);

	float currentDeathMontageTimeFraction = Montage_GetTimeFraction(mDeathMontage);
	if (Montage_IsPlaying(mDeathMontage) && currentDeathMontageTimeFraction == 1.0f && currentDeathMontageTimeFraction > mPrevDeathMontageTimeFraction)
	{
		OnEndDeath.Broadcast();
	}
	mPrevDeathMontageTimeFraction = currentDeathMontageTimeFraction;
}

bool UGodfallCharacterAnimInstanceBase::IgnoreDamage() const
{
	if (ensure(mDeathMontage) && Montage_IsPlaying(mDeathMontage)) return true;
	return false;
}

bool UGodfallCharacterAnimInstanceBase::DamageToAdditive() const
{
	if (ensure(mStunMontage) && Montage_IsPlaying(mStunMontage)) return true;
	return false;
}

void UGodfallCharacterAnimInstanceBase::PlayDamageMontage(EDamage damage, EDamageDirection direction)
{
	if (!ensure(mDamageMontage)) return;

	if (IgnoreDamage()) return;
	
	if (DamageToAdditive())
	{
		PlayAdditiveDamageMontage(direction);
		return;
	}

	bool isPlayingDamage = Montage_IsPlaying(mDamageMontage);
	bool playAdditive = false;

	if (isPlayingDamage)
	{
		EDamage currentDamageType = GetPlayingDamage();

		switch (currentDamageType)
		{
			case EDamage::Flinch:
			{
				playAdditive = (damage == EDamage::Flinch);
			}
			break;
			case EDamage::Stagger:
			{
				playAdditive = (damage == EDamage::Flinch);
			}
			break;
			case EDamage::Knockback:
			{
				playAdditive = (damage == EDamage::Flinch);
			}
			break;
		}
	}

	if (playAdditive)
	{
		PlayAdditiveDamageMontage(direction);
		return;
	}

	FName section;
	switch (damage)
	{
		case EDamage::Flinch:
		{
			if (direction == EDamageDirection::Left)
			{
				int index = FMath::RandRange(0, mDamageMontageSection.Flinch_Left_Names.Num() - 1);
				section = mDamageMontageSection.Flinch_Left_Names[index];
			}
			else
			{
				int index = FMath::RandRange(0, mDamageMontageSection.Flinch_Right_Names.Num() - 1);
				section = mDamageMontageSection.Flinch_Right_Names[index];
			}
		}
		break;
		case EDamage::Stagger:
		{
			if (direction == EDamageDirection::Left)
			{
				int index = FMath::RandRange(0, mDamageMontageSection.Stagger_Left_Names.Num() - 1);
				section = mDamageMontageSection.Stagger_Left_Names[index];
			}
			else
			{
				int index = FMath::RandRange(0, mDamageMontageSection.Stagger_Right_Names.Num() - 1);
				section = mDamageMontageSection.Stagger_Right_Names[index];
			}
		}
		break;
		case EDamage::Knockback:
		{
			int index = FMath::RandRange(0, mDamageMontageSection.Knockback_Names.Num() - 1);
			section = mDamageMontageSection.Knockback_Names[index];
		}
		break;
	}

	if (!ensureMsgf(mDamageMontage->IsValidSectionName(section), TEXT("Is valid section: %s"), *section.ToString())) return;

	StopAllMontages(0.0f);
	Montage_PlayAtSection(mDamageMontage, section);
}

void UGodfallCharacterAnimInstanceBase::PlayAdditiveDamageMontage(EDamageDirection direction)
{
	if (!ensure(mAdditiveFlinchMontage)) return;

	if (IgnoreDamage()) return;

	FName section;

	if (direction == EDamageDirection::Left)
	{
		int index = FMath::RandRange(0, mDamageMontageSection.Flinch_Left_Names.Num() - 1);
		section = mDamageMontageSection.Flinch_Left_Names[index];
	}
	else
	{
		int index = FMath::RandRange(0, mDamageMontageSection.Flinch_Right_Names.Num() - 1);
		section = mDamageMontageSection.Flinch_Right_Names[index];
	}
	
	if (!ensure(mDamageMontage->IsValidSectionName(section))) return;
	Montage_PlayAtSection(mAdditiveFlinchMontage, section);
}

EDamage UGodfallCharacterAnimInstanceBase::GetPlayingDamage() const
{
	if (!ensure(mDamageMontage)) return EDamage::None;
	if (!Montage_IsPlaying(mDamageMontage)) return EDamage::None;

	EDamage currentDamageType = EDamage::None;
	FName currentSection = Montage_GetCurrentSection(mDamageMontage);
	if (mDamageMontageSection.Flinch_Left_Names.Contains(currentSection)) currentDamageType = EDamage::Flinch;
	else if (mDamageMontageSection.Flinch_Right_Names.Contains(currentSection)) currentDamageType = EDamage::Flinch;
	else if (mDamageMontageSection.Stagger_Left_Names.Contains(currentSection)) currentDamageType = EDamage::Stagger;
	else if (mDamageMontageSection.Stagger_Right_Names.Contains(currentSection)) currentDamageType = EDamage::Stagger;
	else if (mDamageMontageSection.Knockback_Names.Contains(currentSection)) currentDamageType = EDamage::Knockback;
	else currentDamageType = EDamage::Other;
	return currentDamageType;
}

void UGodfallCharacterAnimInstanceBase::PlayDeathMontage()
{
	if (!ensure(mDeathMontage)) return;

	mDeathMontage->bEnableAutoBlendOut = false;
	StopAllMontages(0.0f);
	Montage_Play(mDeathMontage);
}

void UGodfallCharacterAnimInstanceBase::PlayStunMontage()
{
	if (!ensure(mStunMontage)) return;

	StopAllMontages(0.0f);
	Montage_Play(mStunMontage);
}

bool UGodfallCharacterAnimInstanceBase::IsPlayingStunMontage() const
{
	if (!ensure(mStunMontage)) return false;
	return Montage_IsPlaying(mStunMontage);
}

void UGodfallCharacterAnimInstanceBase::AnimNotifyState_CanComboInput(
	EAnimNotifyStateEvent eEvent, const UGodfallAnimNotifyStateBase* animNotifyState, 
	const FAnimNotifiyStateData& data)
{
	switch (eEvent)
	{
		// 콤보 가능 구간 애니메이션 노티파이 스테이트가 시작되면, 플래그를 활성화합니다.
		case EAnimNotifyStateEvent::Begin:
			mCanComboInput = true;
			break;
			// 콤보 가능 구간 애니메이션 노티파이 스테이트가 종료되면, 플래그를 비활성화합니다.
		case EAnimNotifyStateEvent::End:
			mCanComboInput = false;
			break;
	}
}

void UGodfallCharacterAnimInstanceBase::AnimNotifyState_Rotate(EAnimNotifyStateEvent eEvent, const UGodfallAnimNotifyStateBase* animNotifyState, const FAnimNotifiyStateData& data)
{
	if (eEvent == EAnimNotifyStateEvent::Tick) return;

	const UAnimNotifyState_Rotate* rotateNotify = Cast<const UAnimNotifyState_Rotate>(animNotifyState);
	if (!rotateNotify) return;

	switch (eEvent)
	{
		case EAnimNotifyStateEvent::Begin:
			OnRotateEvent.Broadcast(eEvent, rotateNotify->GetOverrideData());
			break;
		case EAnimNotifyStateEvent::End:
			OnRotateEvent.Broadcast(eEvent, rotateNotify->GetNoOverrideData());
			break;
	}
}

void UGodfallCharacterAnimInstanceBase::AnimNotifyState_Damagable(EAnimNotifyStateEvent eEvent, const UGodfallAnimNotifyStateBase* animNotifyState, const FAnimNotifiyStateData& data)
{
	if (eEvent == EAnimNotifyStateEvent::Tick) return;

	const UAnimNotifyState_Damagable* damagableNotify = Cast<const UAnimNotifyState_Damagable>(animNotifyState);
	if (!damagableNotify) return;

	OnDamagableEvent.Broadcast(eEvent, damagableNotify->GetData());
}

void UGodfallCharacterAnimInstanceBase::OnMontageStartedCallback(UAnimMontage* montage)
{
	Super::OnMontageStartedCallback(montage);

	if (montage == mStunMontage)
	{
		OnBeginStun.Broadcast();
	}
}

void UGodfallCharacterAnimInstanceBase::OnMontageEndedCallback(UAnimMontage* montage, bool interrupted)
{
	Super::OnMontageEndedCallback(montage, interrupted);

	if (montage == mStunMontage && !Montage_IsPlaying(mStunMontage))
	{
		OnEndStun.Broadcast(interrupted);
	}
}
