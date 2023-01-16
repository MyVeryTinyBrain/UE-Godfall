// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "GodfallAnimNotifyStateBase.h"
#include "Animation/AnimInstance.h"
#include "AnimNotifyState_Custom.h"
#include "GodfallAnimInstanceBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCustomAnimNotifyState, EAnimNotifyStateEvent, eEvent, const FAnimNotifiyStateData&, data, const FName&, notifyStateName, const FCustomNotifyStateVariable&, variables);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnimNotifyFootstep);

/* 몽타주 섹션 트랜지션을 정의하는 타입입니다.
여러 섹션에서 어느 하나의 섹션으로의 트랜지션은 가능하나,
한 섹션에서 여러 섹션으로의 트랜지션은 불가능합니다.
필요하다면 직접 섹션 전환을 사용하세요. */
struct FMontageSectionTransition
{
	UAnimMontage* Montage = nullptr;
	FName From = NAME_None;
	FName To = NAME_None;

	/* 섹션간 전환에 사용되는 블렌드 시간입니다.
	이 값이 0보다 작다면 몽타주의 Blend In 시간이 사용됩니다. */
	float BlendInTime = -1.0f;

	/* 트랜지션 구분에 사용되는 이름입니다.
	사용하지 않아도 괜찮습니다. */
	FName Name = NAME_None;

	bool IsValid() const;
	bool Equal(const FMontageSectionTransition& other) const;
	bool operator == (const FMontageSectionTransition& rhs) const { return Equal(rhs); }
	bool operator != (const FMontageSectionTransition& rhs) const { return !Equal(rhs); }
};

/**
 * 
 */
UCLASS()
class GODFALL_API UGodfallAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()

public:
	UGodfallAnimInstanceBase();

protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeThreadSafeUpdateAnimation(float deltaTime) override;
	
public:
	FName GetCurrentStateName(FString stateMachineName) const;
	float GetTimeFraction(FString stateMachineName, FString stateName) const;
	float GetBlendWeight(FString stateMachineName) const;
	float GetBlendWeight(FString stateMachineName, FString stateName) const;

	float Montage_GetTimeFraction(const UAnimMontage* montage) const;
	float Montage_GetTimeFraction(const UAnimMontage* montage, const FName& sectionName) const;
	void Montage_SetTimeFraction(const UAnimMontage* montage, float timeFraction);
	void Montage_SetTimeFraction(const UAnimMontage* montage, const FName& sectionName, float timeFraction);
	bool Montage_IsPlayingSection(const UAnimMontage* montage, const FName& sectionName) const;

	UFUNCTION(BlueprintPure, Meta = (BlueprintThreadSafe))
	bool Montage_IsPlaying_Threadsafe(const UAnimMontage* montage) const;

	UFUNCTION(BlueprintPure, Meta = (BlueprintThreadSafe))
	bool IsAnyMontagePlaying_Threadsafe() const { return (mPlayingMontages.Num() > 0); }

	float Montage_PlayAtSection(UAnimMontage* MontageToPlay, FName section, float blendInTime = -1.f, float InPlayRate = 1.f, EMontagePlayReturnType ReturnValueType = EMontagePlayReturnType::MontageLength, bool bStopAllMontages = true);
	float Montage_PlayAtSection(UAnimMontage* MontageToPlay, int32 section, float blendInTime = -1.f, float InPlayRate = 1.f, EMontagePlayReturnType ReturnValueType = EMontagePlayReturnType::MontageLength, bool bStopAllMontages = true);

	void AddManualMontageSectionTransition(const FMontageSectionTransition& transition);
	void AddManualMontageSectionTransition(UAnimMontage* Montage, FName From, FName To, float BlendInTime = -1.0f, FName Name = NAME_None);
	bool RemoveManualMontageSection(const FMontageSectionTransition* transition);
	const FMontageSectionTransition* FindManualMontageSectionTransitionByName(UAnimMontage* Montage, FName Name) const;
	const FMontageSectionTransition* FindManualMontageSectionTransitionByFrom(UAnimMontage* Montage, FName From) const;
	const FMontageSectionTransition* Montage_ManualSectionTransit(UAnimMontage* Montage);
	const FMontageSectionTransition* Montage_PlayOrManualSectionTransit(UAnimMontage* Montage, FName section = NAME_None, float blendInTime = -1.f, float InPlayRate = 1.f, EMontagePlayReturnType ReturnValueType = EMontagePlayReturnType::MontageLength, bool bStopAllMontages = true, float* result = nullptr);

public:
	bool IsExitable() const { return mExitable > 0; }
	bool IsTransitable() const { return mTransitable > 0; }

protected:
	UFUNCTION()
	virtual void AnimNotifyState_Custom(EAnimNotifyStateEvent eEvent, const UGodfallAnimNotifyStateBase* animNotifyState, const FAnimNotifiyStateData& data);

	virtual void OnCustomAnimNotifyStateCallback(EAnimNotifyStateEvent eEvent, const FAnimNotifiyStateData& data, const FName& notifyStateName, const FCustomNotifyStateVariable& variables) {}

	UFUNCTION()
	virtual void AnimNotifyState_Exitable(EAnimNotifyStateEvent eEvent, const UGodfallAnimNotifyStateBase* animNotifyState, const FAnimNotifiyStateData& data);
	
	UFUNCTION()
	virtual void AnimNotifyState_Transitable(EAnimNotifyStateEvent eEvent, const UGodfallAnimNotifyStateBase* animNotifyState, const FAnimNotifiyStateData& data);

	UFUNCTION()
	virtual void OnMontageStartedCallback(UAnimMontage* montage);

	UFUNCTION()
	virtual void OnMontageEndedCallback(UAnimMontage* montage, bool interrupted);

	UFUNCTION()
	virtual void AnimNotify_FootStep();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "Exitable")
	int32 mExitable = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "Transitable")
	int32 mTransitable = 0;

	TArray<UAnimMontage*> mPlayingMontages;

	/* TMap<섹션 전환을 할 몽타주, TMap<시작 섹션, 몽타주 섹션 트랜지션>> */
	TMap<UAnimMontage*, TMap<FName, FMontageSectionTransition>> mManualMontageSectionTransitions;

public:
	FOnCustomAnimNotifyState OnCustomAnimNotifyState;
	FOnAnimNotifyFootstep OnAnimNotifyFootstep;
};
