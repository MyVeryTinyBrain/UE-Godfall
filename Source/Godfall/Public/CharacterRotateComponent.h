// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "RotateTypes.h"
#include "Components/ActorComponent.h"
#include "CharacterRotateComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GODFALL_API UCharacterRotateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterRotateComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	

public:
	void ActiveRotate(FRotator initRotator);
	void DeactiveRotate();
	void SetRotator(FRotator rotator);
	void SetRotatorDirect(FRotator rotator);
	void ResetRotator();

	bool GetOverridedRotate() const;
	float GetOverridedSpeed() const;
	FRotator GetOverridedRotator() const;

	bool IsTarget(const FString& TargetName) const;
	bool AddActorTarget(const FString& TargetName, TWeakObjectPtr<const AActor> actor);
	bool AddLocationTarget(const FString& TargetName, const FVector& location);
	bool AddDirectionTarget(const FString& TargetName, const FVector& direction);
	bool UpdateActorTarget(const FString& TargetName, TWeakObjectPtr<const AActor> actor);
	bool UpdateLocationTarget(const FString& TargetName, const FVector& location);
	bool UpdateDirectionTarget(const FString& TargetName, const FVector& direction);
	bool RemoveTarget(const FString& TargetName);

private:
	void Rotate(float deltaTime);

public:
	UPROPERTY(EditAnywhere, DisplayName = "Rotate")
	bool mRotate = true;

	UPROPERTY(EditAnywhere, DisplayName = "Rotate On XY Coordinates")
	bool mRotateOnXYCoordinates = true;

	UPROPERTY(EditAnywhere, DisplayName = "Rotate Speed", Meta = (ClampMin = 0.0f))
	float mRotateSpeed = 10.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Rotator")
	FRotator mRotator = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, DisplayName = "Rotate Override")
	FRotateOverride mOverride = FRotateOverride::NoOverride();

private:
	UPROPERTY(VisibleAnywhere, DisplayName = "Actor Targets", Meta = (AllowPrivateAccess))
	TMap<FString, TWeakObjectPtr<const AActor>> mActorTargets;

	UPROPERTY(VisibleAnywhere, DisplayName = "Location Targets", Meta = (AllowPrivateAccess))
	TMap<FString, FVector> mLocationTargets;

	UPROPERTY(VisibleAnywhere, DisplayName = "Direction Targets", Meta = (AllowPrivateAccess))
	TMap<FString, FVector> mDirectionTargets;
};
