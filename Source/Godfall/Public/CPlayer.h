// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CSilvermane.h"
#include "CPlayer.generated.h"

/**
 * 
 */
UCLASS()
class GODFALL_API ACPlayer : public ACSilvermane
{
	GENERATED_BODY()
	
public:

	ACPlayer();

private:
	virtual void SetupPlayerInputComponent(class UInputComponent* inputComp) override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	virtual void OnDead() override {}

	virtual void OnHealthChanged(float prev, float changed) override;
	virtual void OnSteminaChanged(float prev, float changed) override;

private:
	void InputMoveForwardBackward(float input);
	void InputMoveRightLeft(float input);
	void InputTurnRightLeft(float input);
	void InputLookUpDown(float input);
	void InputWalk(float input);
	void InputSprint(float input);
	void InputSlidePressed();
	void InputGuard(float input);
	void InputAttackPressed();
	void InputAttackReleased();
	void InputChangeWeapon(float input);
	void InputTestPressed();
	void InputLockOnPressed();
	bool OnLightAttack();
	bool OnHeavyAttack();

	void RotateToCameraForward();
	void RotateToMoveDirection();
	void AccAttackPressed();
	void ToggleLockOn();
	void TryChangeLockOnTarget(float deltaTime, const FVector2D& screenDelta);

public:
	void UpdatePlayerHUD();

public:
	UPROPERTY(EditAnywhere, DisplayName = "Heavy Attack Accumulation Time", Category = "Actor")
	float mHeavyAttackAccTime = 0.2f;

	UPROPERTY(EditAnywhere, DisplayName = "Delay To Change LockOn", Category = "Actor", Meta = (ClampMin = 0.1f))
	float mDelayToChnageLockOn = 0.2f;

	UPROPERTY(EditAnywhere, DisplayName = "Limit Delta Time To Change LockOn", Category = "Actor", Meta = (ClampMin = 1.0f))
	float mLimitDeltaLengthToChangeLockOn = 4.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Interp, Category = "Actor")
	bool ShowHUD = true;

private:
	UPROPERTY(VisibleAnywhere, DisplayName = "Camera Director", Category = "Actor")
	class ACameraDirector* mCameraDirector;

	bool mAttackPressed = false;
	float mAttackPressedAcc = 0.0f;

	float mChangeLockOnDelay = 0.0f;
	FVector2D mMouseDelta = FVector2D::ZeroVector;
};
