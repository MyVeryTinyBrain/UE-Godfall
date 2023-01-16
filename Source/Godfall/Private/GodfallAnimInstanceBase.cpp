// Fill out your copyright notice in the Description page of Project Settings.


#include "GodfallAnimInstanceBase.h"
#include <Animation/AnimNode_StateMachine.h>

UGodfallAnimInstanceBase::UGodfallAnimInstanceBase()
{
	RootMotionMode = ERootMotionMode::RootMotionFromEverything;
}

void UGodfallAnimInstanceBase::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	OnMontageStarted.AddDynamic(this, &UGodfallAnimInstanceBase::OnMontageStartedCallback);
	OnMontageEnded.AddDynamic(this, &UGodfallAnimInstanceBase::OnMontageEndedCallback);
}

void UGodfallAnimInstanceBase::NativeThreadSafeUpdateAnimation(float deltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(deltaTime);
}

FName UGodfallAnimInstanceBase::GetCurrentStateName(FString stateMachineName) const
{
	const FAnimNode_StateMachine* stateMachine = GetStateMachineInstanceFromName(FName(stateMachineName));
	if (stateMachine)
	{
		int32 currentStateIndex = stateMachine->GetCurrentState();
		const FBakedAnimationState& bakedAnimationState = stateMachine->GetStateInfo(currentStateIndex);
		return bakedAnimationState.StateName;
	}
	return FName();
}

float UGodfallAnimInstanceBase::GetTimeFraction(FString stateMachineName, FString stateName) const
{
	const FAnimNode_StateMachine* stateMachine = GetStateMachineInstanceFromName(FName(stateMachineName));
	if (stateMachine)
	{
		int32 stateMachineIndex = GetStateMachineIndex(FName(stateMachineName));
		if (stateMachineIndex < 0) return 0.0f;
		const FBakedAnimationStateMachine* desc = GetStateMachineInstanceDesc(FName(stateMachineName));
		int32 stateIndex = desc->FindStateIndex(FName(stateName));
		if (stateIndex < 0) return 0.0f;
		const UAnimInstance* animInstance = this;
		return const_cast<UAnimInstance*>(animInstance)->GetRelevantAnimTimeFraction(stateMachineIndex, stateIndex);
	}
	
	return 0.0f;
}

float UGodfallAnimInstanceBase::GetBlendWeight(FString stateMachineName) const
{
	const FAnimNode_StateMachine* stateMachine = GetStateMachineInstanceFromName(FName(stateMachineName));
	if (stateMachine)
	{
		int32 stateMachineIndex = GetStateMachineIndex(FName(stateMachineName));
		const UAnimInstance* animInstance = this;
		return const_cast<UAnimInstance*>(animInstance)->GetInstanceMachineWeight(stateMachineIndex);
	}

	return 0.0f;
}

float UGodfallAnimInstanceBase::GetBlendWeight(FString stateMachineName, FString stateName) const
{
	const FAnimNode_StateMachine* stateMachine = GetStateMachineInstanceFromName(FName(stateMachineName));
	if (stateMachine)
	{
		int32 stateMachineIndex = GetStateMachineIndex(FName(stateMachineName));
		const FBakedAnimationStateMachine* desc = GetStateMachineInstanceDesc(FName(stateMachineName));
		int32 stateIndex = desc->FindStateIndex(FName(stateName));
		if (stateIndex < 0) return 0.0f;
		const UAnimInstance* animInstance = this;
		return const_cast<UAnimInstance*>(animInstance)->GetInstanceStateWeight(stateMachineIndex, stateIndex);
	}
	
	return 0.0f;
}

float UGodfallAnimInstanceBase::Montage_GetTimeFraction(const UAnimMontage* montage) const
{
	if (!Montage_IsPlaying(montage))
	{
		return -1.0f;
	}

	return Montage_GetPosition(montage) / montage->GetPlayLength();
}

float UGodfallAnimInstanceBase::Montage_GetTimeFraction(const UAnimMontage* montage, const FName& sectionName) const
{
	if (!Montage_IsPlaying(montage))
	{
		return -1.0f;
	}

	if (!ensure(montage->IsValidSectionName(sectionName)))
	{
		return -1.0f;
	}

	int sectionIndex = montage->GetSectionIndex(sectionName);

	float sectionStart, sectionEnd;
	montage->GetSectionStartAndEndTime(sectionIndex, sectionStart, sectionEnd);

	float sectionDelta = sectionEnd - sectionStart;
	float currentPosition = Montage_GetPosition(montage);
	float currentSectionRate = (currentPosition - sectionStart) / sectionDelta;

	return currentSectionRate;
}

void UGodfallAnimInstanceBase::Montage_SetTimeFraction(const UAnimMontage* montage, float timeFraction)
{
	if (!Montage_IsPlaying(montage))
	{
		return;
	}

	float newPosition = montage->GetPlayLength() * timeFraction;
	Montage_SetPosition(montage, newPosition);
}

void UGodfallAnimInstanceBase::Montage_SetTimeFraction(const UAnimMontage* montage, const FName& sectionName, float timeFraction)
{
	if (!Montage_IsPlaying(montage))
	{
		return;
	}

	if (!ensure(montage->IsValidSectionName(sectionName)))
	{
		return;
	}

	FName currentSectionName = Montage_GetCurrentSection(montage);
	int currentSectionIndex = montage->GetSectionIndex(currentSectionName);

	float sectionStart, sectionEnd;
	montage->GetSectionStartAndEndTime(currentSectionIndex, sectionStart, sectionEnd);

	float sectionDelta = sectionEnd - sectionStart;
	float newPosition = sectionStart + sectionDelta * timeFraction;
	Montage_SetPosition(montage, newPosition);
}

bool UGodfallAnimInstanceBase::Montage_IsPlayingSection(const UAnimMontage* montage, const FName& sectionName) const
{
	float timeFractionInSection = Montage_GetTimeFraction(montage, sectionName);
	if (timeFractionInSection < 0.0f || timeFractionInSection > 1.0f)
	{
		return false;
	}

	return true;
}

bool UGodfallAnimInstanceBase::Montage_IsPlaying_Threadsafe(const UAnimMontage* montage) const
{
	if (!montage) return IsAnyMontagePlaying_Threadsafe();
	else return (INDEX_NONE != mPlayingMontages.Find(const_cast<UAnimMontage*>(montage)));
}

float UGodfallAnimInstanceBase::Montage_PlayAtSection(UAnimMontage* MontageToPlay, FName section, float blendInTime, float InPlayRate, EMontagePlayReturnType ReturnValueType, bool bStopAllMontages)
{
	if (!MontageToPlay) return 0.0f;

	if (!MontageToPlay->IsValidSectionName(section)) return 0.0f;

	int32 sectionIndex = MontageToPlay->GetSectionIndex(section);
	return Montage_PlayAtSection(MontageToPlay, sectionIndex, blendInTime, InPlayRate, ReturnValueType, bStopAllMontages);
}

float UGodfallAnimInstanceBase::Montage_PlayAtSection(UAnimMontage* MontageToPlay, int32 section, float blendInTime, float InPlayRate, EMontagePlayReturnType ReturnValueType, bool bStopAllMontages)
{
	if (!MontageToPlay) return 0.0f;

	if (!MontageToPlay->IsValidSectionIndex(section)) return 0.0f;

	float startTime, endTime;
	MontageToPlay->GetSectionStartAndEndTime(section, startTime, endTime);

	if (blendInTime < 0.0f)
	{
		return Montage_Play(MontageToPlay, InPlayRate, ReturnValueType, startTime, bStopAllMontages);
	}
	else
	{
		FMontageBlendSettings settings(blendInTime);
		return Montage_PlayWithBlendSettings(MontageToPlay, settings, InPlayRate, ReturnValueType, startTime, bStopAllMontages);
	}
}

void UGodfallAnimInstanceBase::AddManualMontageSectionTransition(const FMontageSectionTransition& transition)
{
	if (!ensure(transition.IsValid())) return;

	TMap<FName, FMontageSectionTransition>& transitions = mManualMontageSectionTransitions.FindOrAdd(transition.Montage);
	transitions.Add(transition.From, transition);
}

void UGodfallAnimInstanceBase::AddManualMontageSectionTransition(UAnimMontage* Montage, FName From, FName To, float BlendInTime, FName Name)
{
	FMontageSectionTransition transition;
	transition.Montage = Montage;
	transition.From = From;
	transition.To = To;
	transition.BlendInTime = BlendInTime;
	transition.Name = Name;
	AddManualMontageSectionTransition(transition);
}

bool UGodfallAnimInstanceBase::RemoveManualMontageSection(const FMontageSectionTransition* transition)
{
	if (!transition) return false;

	TMap<FName, FMontageSectionTransition>* pTransitions = mManualMontageSectionTransitions.Find(transition->Montage);
	if (!pTransitions) return false;

	bool succeeded = pTransitions->Remove(transition->From) > 0;
	if (succeeded && pTransitions->IsEmpty())
	{
		mManualMontageSectionTransitions.Remove(transition->Montage);
	}

	return succeeded;
}

const FMontageSectionTransition* UGodfallAnimInstanceBase::FindManualMontageSectionTransitionByName(UAnimMontage* Montage, FName Name) const
{
	const TMap<FName, FMontageSectionTransition>* pTransitions = mManualMontageSectionTransitions.Find(Montage);
	if (!pTransitions) return nullptr;

	for (auto& transitionPair : *pTransitions)
	{
		if (transitionPair.Value.Name == Name)
		{
			return &transitionPair.Value;
		}
	}

	return nullptr;
}

const FMontageSectionTransition* UGodfallAnimInstanceBase::FindManualMontageSectionTransitionByFrom(UAnimMontage* Montage, FName From) const
{
	const TMap<FName, FMontageSectionTransition>* pTransitions = mManualMontageSectionTransitions.Find(Montage);
	if (!pTransitions) return nullptr;

	for (auto& transitionPair : *pTransitions)
	{
		if (transitionPair.Value.From == From)
		{
			return &transitionPair.Value;
		}
	}

	return nullptr;
}

const FMontageSectionTransition* UGodfallAnimInstanceBase::Montage_ManualSectionTransit(UAnimMontage* Montage)
{
	if (!Montage) return nullptr;

	if (!Montage_IsPlaying(Montage)) return nullptr;
	FName currentSection = Montage_GetCurrentSection(Montage);

	const TMap<FName, FMontageSectionTransition>* pTransitions = mManualMontageSectionTransitions.Find(Montage);
	if (!pTransitions) return nullptr;

	const FMontageSectionTransition* transition = pTransitions->Find(currentSection);
	if (!transition) return nullptr;

	if (0 == Montage_PlayAtSection(Montage, transition->To, transition->BlendInTime)) return nullptr;

	return transition;
}

const FMontageSectionTransition* UGodfallAnimInstanceBase::Montage_PlayOrManualSectionTransit(UAnimMontage* Montage, FName section, float blendInTime, float InPlayRate, EMontagePlayReturnType ReturnValueType, bool bStopAllMontages, float* result)
{
	if (!Montage) return nullptr;

	if (!Montage_IsPlaying(Montage))
	{
		if (section.IsNone())
		{
			if (blendInTime < 0.0f)
			{
				float playResult = Montage_Play(Montage, InPlayRate, ReturnValueType, 0.0f, bStopAllMontages);
				if (result) *result = playResult;
			}
			else
			{
				FMontageBlendSettings settings(blendInTime);
				float playResult = Montage_PlayWithBlendSettings(Montage, settings, InPlayRate, ReturnValueType, 0.0f, bStopAllMontages);
				if (result) *result = playResult;
			}
		}
		else
		{
			if (result) *result = Montage_PlayAtSection(Montage, section, blendInTime, InPlayRate, ReturnValueType, bStopAllMontages);
		}

		return nullptr;
	}
	else
	{
		if (result) *result = 0.0f;
		return Montage_ManualSectionTransit(Montage);
	}
}

void UGodfallAnimInstanceBase::AnimNotifyState_Custom(EAnimNotifyStateEvent eEvent, const UGodfallAnimNotifyStateBase* animNotifyState, const FAnimNotifiyStateData& data)
{
	const UAnimNotifyState_Custom* customNotifyState = Cast<const UAnimNotifyState_Custom>(animNotifyState);
	if (customNotifyState)
	{
		OnCustomAnimNotifyStateCallback(eEvent, data, customNotifyState->GetNotifyStateName(), customNotifyState->GetCustomVariables());
		OnCustomAnimNotifyState.Broadcast(eEvent, data, customNotifyState->GetNotifyStateName(), customNotifyState->GetCustomVariables());
	}
}

void UGodfallAnimInstanceBase::AnimNotifyState_Exitable(EAnimNotifyStateEvent eEvent, const UGodfallAnimNotifyStateBase* animNotifyState, const FAnimNotifiyStateData& data)
{
	if (eEvent == EAnimNotifyStateEvent::Tick) return;
	
	switch (eEvent)
	{
		case EAnimNotifyStateEvent::Begin:
			//mExitable = true;
			++mExitable;
			break;
		case EAnimNotifyStateEvent::End:
			//mExitable = false;
			--mExitable;
			break;
	}
}

void UGodfallAnimInstanceBase::AnimNotifyState_Transitable(EAnimNotifyStateEvent eEvent, const UGodfallAnimNotifyStateBase* animNotifyState, const FAnimNotifiyStateData& data)
{
	if (eEvent == EAnimNotifyStateEvent::Tick) return;

	switch (eEvent)
	{
		case EAnimNotifyStateEvent::Begin:
			++mTransitable;
			break;
		case EAnimNotifyStateEvent::End:
			--mTransitable;
			break;
	}
}

void UGodfallAnimInstanceBase::OnMontageStartedCallback(UAnimMontage* montage)
{
	mPlayingMontages.Add(montage);
}

void UGodfallAnimInstanceBase::OnMontageEndedCallback(UAnimMontage* montage, bool interrupted)
{
	mPlayingMontages.RemoveSingle(montage);
}

void UGodfallAnimInstanceBase::AnimNotify_FootStep()
{
	OnAnimNotifyFootstep.Broadcast();
}

bool FMontageSectionTransition::IsValid() const
{
	if (!Montage) return false;
	if (From.IsNone()) return false;
	if (To.IsNone()) return false;

	return true;
}

bool FMontageSectionTransition::Equal(const FMontageSectionTransition& other) const
{
	if (Montage != other.Montage) return false;
	if (From != other.From) return false;
	if (To != other.To) return false;
	if (BlendInTime != other.BlendInTime) return false;
	if (Name != other.Name) return false;

	return true;
}
