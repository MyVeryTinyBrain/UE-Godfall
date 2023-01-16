// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallEnemyBase.h"
#include "CSolaris.generated.h"

class ABeam;

/**
 * 
 */
UCLASS()
class GODFALL_API ACSolaris : public AGodfallEnemyBase
{
	GENERATED_BODY()
	
public:
	ACSolaris();

protected:
	virtual void PostInitializeComponents() override;
	virtual void SetupStateTable(FStateTable& mStateTable) override;
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	virtual void OnDead() override;

private:
	UFUNCTION()
	void OnCustomAnimNotifyState(EAnimNotifyStateEvent eEvent, const FAnimNotifiyStateData& data, const FName& notifyStateName, const FCustomNotifyStateVariable& variables);

	void SpawnBeamRainToTarget(float radius, float delay, float duration, float width);
	void SpawnPlasmaZoneOnGround(float radius, float duration);

private:
	UPROPERTY()
	class UAnimInstanceSolaris* mAnimInstance = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	UCapsuleComponent* WeaponRHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	UCapsuleComponent* BigWeaponRHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	UCapsuleComponent* ArmRHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	UCapsuleComponent* BigHandLHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	ABeam* Beam;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Beam Rain Interval", Category = "Actor", Meta = (ClampMin = 0.0f))
	float mBeamRainInterval = 0.4f;

private:
	float mBeamRainAccumulation = 0.0f;
};
