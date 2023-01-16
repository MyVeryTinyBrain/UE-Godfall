// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallEnemyBase.h"
#include "CGrievesMinion.generated.h"

/**
 * 
 */
UCLASS()
class GODFALL_API ACGrievesMinion : public AGodfallEnemyBase
{
	GENERATED_BODY()

public:
	ACGrievesMinion();

protected:
	virtual void PostInitializeComponents() override;
	virtual void SetupStateTable(FStateTable& mStateTable) override;
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

private:
	UPROPERTY()
	class UAnimInstanceEarthAberrant* mAnimInstance = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	USceneComponent* WeaponRSocketComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	UCapsuleComponent* WeaponHitbox;
};
