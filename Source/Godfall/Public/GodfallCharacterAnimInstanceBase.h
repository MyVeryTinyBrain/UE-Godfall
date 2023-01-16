// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageTypes.h"
#include "RotateTypes.h"
#include "GodfallAnimInstanceBase.h"
#include "GodfallCharacterAnimInstanceBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamagableEvent, EAnimNotifyStateEvent, eEvent, const FDamagableData&, data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRotateEvent, EAnimNotifyStateEvent, eEvent, const FRotateOverride&, data);
DECLARE_MULTICAST_DELEGATE(FOnBeginStun);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEndStun, bool interrupted);
DECLARE_MULTICAST_DELEGATE(FOnEndDeath);

USTRUCT(BlueprintType, Blueprintable)
struct FDamageMontageSection
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Flinch_Left_Names = { TEXT("Flinch_Left") };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Flinch_Right_Names = { TEXT("Flinch_Right") };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Stagger_Left_Names = { TEXT("Stagger_Left") };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Stagger_Right_Names = { TEXT("Stagger_Right") };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Knockback_Names = { TEXT("Knockback") };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Additive_Flinch_Left_Names = { TEXT("Flinch_Left") };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Additive_Flinch_Right_Names = { TEXT("Flinch_Right") };
};

/**
 * 
 */
UCLASS()
class GODFALL_API UGodfallCharacterAnimInstanceBase : public UGodfallAnimInstanceBase
{
	GENERATED_BODY()

protected:
	virtual void NativeThreadSafeUpdateAnimation(float deltaTime) override;
	
public:
	bool CanComboInput() const { return mCanComboInput; }

	virtual bool IgnoreDamage() const;
	virtual bool DamageToAdditive() const;

	void PlayDamageMontage(EDamage damage, EDamageDirection direction);
	void PlayAdditiveDamageMontage(EDamageDirection direction);
	EDamage GetPlayingDamage() const;
	void PlayDeathMontage();
	void PlayStunMontage();

	bool IsPlayingStunMontage() const;

protected:
	UFUNCTION()
	virtual void AnimNotifyState_CanComboInput(EAnimNotifyStateEvent eEvent, const UGodfallAnimNotifyStateBase* animNotifyState, const FAnimNotifiyStateData& data);

	UFUNCTION()
	virtual void AnimNotifyState_Rotate(EAnimNotifyStateEvent eEvent, const UGodfallAnimNotifyStateBase* animNotifyState, const FAnimNotifiyStateData& data);

	UFUNCTION()
	virtual void AnimNotifyState_Damagable(EAnimNotifyStateEvent eEvent, const UGodfallAnimNotifyStateBase* animNotifyState, const FAnimNotifiyStateData& data);

	virtual void OnMontageStartedCallback(UAnimMontage* montage) override;
	virtual void OnMontageEndedCallback(UAnimMontage* montage, bool interrupted) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true), DisplayName = "Damage Montage Section")
	FDamageMontageSection mDamageMontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true), DisplayName = "Damage Montage")
	UAnimMontage* mDamageMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true), DisplayName = "Additive Flinch Montage")
	UAnimMontage* mAdditiveFlinchMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true), DisplayName = "Death Montage")
	UAnimMontage* mDeathMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true), DisplayName = "Stun Montage")
	UAnimMontage* mStunMontage = nullptr;

private:
	bool mCanComboInput = false;
	float mPrevDeathMontageTimeFraction = 0.0f;

public:
	FOnDamagableEvent OnDamagableEvent;
	FOnRotateEvent OnRotateEvent;
	FOnBeginStun OnBeginStun;
	FOnEndStun OnEndStun;
	FOnEndDeath OnEndDeath;
};
