// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallCharacterAnimInstanceBase.h"
#include "GodfallEnemyEnums.h"
#include "GodfallEnemyAnimInstanceBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnBeginEnemyExecution);
DECLARE_MULTICAST_DELEGATE(FOnEndEnemyExecution);

USTRUCT(BlueprintType, Blueprintable)
struct FDashMontageSection
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Dash_Bwd_Names = { TEXT("Dash_Bwd") };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Dash_Left_Names = { TEXT("Dash_Left") };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Dash_Right_Names = { TEXT("Dash_Right") };
};

/**
 * 
 */
UCLASS()
class GODFALL_API UGodfallEnemyAnimInstanceBase : public UGodfallCharacterAnimInstanceBase
{
	GENERATED_BODY()

protected:
	virtual void NativeThreadSafeUpdateAnimation(float deltaTime) override;
	virtual void OnMontageStartedCallback(UAnimMontage* montage) override;

public:
	virtual void PlayDashMontage(EEnemyAvoid3Direction direction);

	void SetMoveSpeed(EEnemyMoveSpeed value) { mMoveSpeed = value; }
	void SetWalkDirection(E4Direction value) { mWalkDirection = value; }

	UAnimMontage* GetExecutionMontage() const { return mExecutionMontage; }
	virtual void PlayExecutionMontageSelf(bool canExit = false);
	bool IsActiveExecutionMontage() const;

	virtual bool IgnoreDamage() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true), DisplayName = "Execution Montage")
	UAnimMontage* mExecutionMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true), DisplayName = "Dash Montage Section")
	FDashMontageSection mDashMontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true), DisplayName = "Dash Montage")
	UAnimMontage* mDashMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "MoveSpeed", Meta = (AllowPrivateAccess))
	EEnemyMoveSpeed mMoveSpeed = EEnemyMoveSpeed::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Walk Direction", Meta = (AllowPrivateAccess))
	E4Direction mWalkDirection = E4Direction::Forward;

	bool mExecutionExit = false;

public:
	FOnBeginEnemyExecution OnBeginExecution;
	FOnEndEnemyExecution OnEndExecution;
};
