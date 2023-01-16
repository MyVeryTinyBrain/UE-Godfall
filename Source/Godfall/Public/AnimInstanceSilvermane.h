// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "GodfallCharacterAnimInstanceBase.h"
#include "CSilvermaneEnums.h"
#include "AnimInstanceSilvermane.generated.h"

class AGodfallEnemyBase;

DECLARE_MULTICAST_DELEGATE(FOnBeginSprint);
DECLARE_MULTICAST_DELEGATE(FOnEndSprint);
DECLARE_MULTICAST_DELEGATE(FOnEquippedOn);
DECLARE_MULTICAST_DELEGATE(FOnEquippedOff);
DECLARE_MULTICAST_DELEGATE(FOnSilvermaneAttack);
DECLARE_MULTICAST_DELEGATE(FOnSilvermaneShield);
DECLARE_MULTICAST_DELEGATE(FOnBeginSilvermaneExecution);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEndSilvermaneExecution, bool interruped);
DECLARE_MULTICAST_DELEGATE(FOnSilvermaneJump);
DECLARE_MULTICAST_DELEGATE(FOnSilvermaneJumpingSection);

/**
 * k
 */
UCLASS()
class GODFALL_API UAnimInstanceSilvermane : public UGodfallCharacterAnimInstanceBase
{
	GENERATED_BODY()

public:
	UAnimInstanceSilvermane();

protected:
	virtual void NativeThreadSafeUpdateAnimation(float deltaTime) override;
	
public:
	void SetMoveDirection(const FVector2D& direction); // X: Horizontal, Y: Vertical
	void SetMoveSpeed(ESilvermaneMoveSpeed value) { mMoveSpeed = value; }
	bool PlaySlideMontage(E4Direction direction);
	bool PlaySlideMontageAsDirection(FVector2D direction);
	bool PlaySlideMontageByCurrentMoveDirection(); // X: Horizontal, Y: Vertical
	void SetGuard(bool value) { mGuardState = value; }
	void SetWeaponType(ESilvermaneWeapon value) { mWeapon = value; }
	ESilvermaneWeapon GetWeaponType() const { return mWeapon; }
	void PlayEquipMontage(ESilvermaneEquipTrigger value);
	void StopEquipOnMontage(float blendingOutTime = 0.0f);
	void StopEquipOffMontage(float blendingOutTime = 0.0f);
	void StopEquipMontage(float blendingOutTime = 0.0f);
	void PlayBlockMontage(ESilvermaneBlock value);
	bool PlayExecutionMontageWithEnemy(AGodfallEnemyBase* enemy);
	void PlayJumpMontage(ESilvermaneJumpType type);
	bool TrySetAttackType(ESilvermaneAttackType type);

	virtual bool IgnoreDamage() const override;
	virtual bool DamageToAdditive() const;

	bool IsPlayingBaseStateMachine() const;
	bool IsActiveEquipOnMontage() const;
	bool IsActiveEquipOffMontage() const;
	bool IsPlayingEquipOnMontage() const;
	bool IsPlayingEquipOffMontage() const;
	bool IsPlayingSprintAnimation() const;
	float GetSprintAnimationBlendWeight() const;
	bool IsActiveAttackMontage() const;
	bool IsPlayingAttackMontage() const;
	bool CanRotate() const;
	bool CanNotSetRotator() const;
	bool CanChangeWeapon() const;
	bool IsShieldOpen() const;
	bool IsActiveExecutionMontage() const;
	bool IsPlayingJumpMontage() const;
	bool IsPlayingJumpingSectionOfJumpMontage() const;
	float GetJumpingSectionTimeFractionOfJumpMontage() const;

private:
	ESilvermaneBlock GetCurrentBlockType() const;

	void SolveEquip();
	void SolveExitable();
	void SolveAttack();

private:
	// BaseStateMachine==========================================================

	// Default
	UFUNCTION()
	void AnimNotify_BeginDefault();

	// Default
	UFUNCTION()
	void AnimNotify_EndDefault();

	// ToShield -> ShieldEnable
	UFUNCTION()
	void AnimNotify_BeginShield();

	// Shield -> ShieldDisable
	UFUNCTION()
	void AnimNotify_EndShield();

	// BaseStateMachine -> Default -> DefaultStateMachine==========================
	// Sprint
	UFUNCTION()
	void AnimNotify_BeginSprint();

	// Sprint
	UFUNCTION()
	void AnimNotify_EndSprint();

	// ===========================================================================

	UFUNCTION()
	void AnimNotify_EquipOn();

	UFUNCTION()
	void AnimNotify_EquipOff();

	UFUNCTION()
	void AnimNotify_StartJumping();

	virtual void OnMontageStartedCallback(UAnimMontage* montage) override;
	virtual void OnMontageEndedCallback(UAnimMontage* montage, bool interrupted) override;

private:
	virtual void AnimNotifyState_CanComboInput(EAnimNotifyStateEvent eEvent, const UGodfallAnimNotifyStateBase* animNotifyState, const FAnimNotifiyStateData& data) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "EquipOn Montage")
	UAnimMontage* mEquipOnMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "EquipOff Montage")
	UAnimMontage* mEquipOffMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "Block Montage")
	UAnimMontage* mBlockMontage;

	// Key: Enemy Class
	// Value: Section Name
	TMap<TSubclassOf<AGodfallEnemyBase>, FString> mExecutionSections;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "Execution Montage")
	UAnimMontage* mExecutionMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "Jump Montage")
	UAnimMontage* mJumpMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "Jump Travel Montage")
	UAnimMontage* mJumpTravelMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "Slide Montage")
	UAnimMontage* mSlideMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "Attack Sword1H Montage")
	UAnimMontage* mAttackSword1HMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "Attack Sword2H Montage")
	UAnimMontage* mAttackSword2HMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "Attack Polearm Montage")
	UAnimMontage* mAttackPolearmMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "Attack DualBlades Montage")
	UAnimMontage* mAttackDualBladesMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "Move Direction")
	FVector2D mMoveDirection = FVector2D(0.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "Move Speed")
	ESilvermaneMoveSpeed mMoveSpeed = ESilvermaneMoveSpeed::Stop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "Weapon")
	ESilvermaneWeapon mWeapon = ESilvermaneWeapon::Sword_1H;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "Guard State")
	bool mGuardState = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "Direct Shield")
	bool mDirectShield = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "Attack Type")
	ESilvermaneAttackType mAttackType;

public:
	FOnBeginSprint OnBeginSprint;
	FOnEndSprint OnEndSprint;
	FOnEquippedOn OnEquippedOn;
	FOnEquippedOff OnEquippedOff;
	FOnSilvermaneAttack OnAttack;
	FOnSilvermaneShield OnShield;
	FOnBeginSilvermaneExecution OnBeginExecution;
	FOnEndSilvermaneExecution OnEndExecution;
	FOnSilvermaneJump OnJump;
	FOnSilvermaneJumpingSection OnJumpingSection;
};
