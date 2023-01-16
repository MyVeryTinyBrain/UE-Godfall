// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterRotateComponent.h"

UCharacterRotateComponent::UCharacterRotateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterRotateComponent::BeginPlay()
{
	Super::BeginPlay();

	ResetRotator();
}

void UCharacterRotateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Rotate(DeltaTime);
}

void UCharacterRotateComponent::ActiveRotate(FRotator initRotator)
{
	if (mRotate) return;
	mRotator = GetOwner()->GetActorRotation();
	mRotate = true;
}

void UCharacterRotateComponent::DeactiveRotate()
{
	mRotate = false;
}

void UCharacterRotateComponent::SetRotator(FRotator rotator)
{
	mRotator = rotator;
}

void UCharacterRotateComponent::SetRotatorDirect(FRotator rotator)
{
	mRotator = rotator;
	GetOwner()->SetActorRotation(rotator, ETeleportType::ResetPhysics);
}

void UCharacterRotateComponent::ResetRotator()
{
	SetRotator(GetOwner()->GetActorRotation());
}

bool UCharacterRotateComponent::GetOverridedRotate() const
{
	return (mOverride.Override && mOverride.OverrideRotate) ? mOverride.Rotate : mRotate;
}

float UCharacterRotateComponent::GetOverridedSpeed() const
{
	return (mOverride.Override && mOverride.OverrideRotateSpeed) ? mOverride.RotateSpeed : mRotateSpeed;
}

FRotator UCharacterRotateComponent::GetOverridedRotator() const
{
	FRotator rotator = mRotator;

	if (mOverride.Override)
	{
		switch (mOverride.OverrideRotator)
		{
			case ERotatorOverrideRotator::NoOverride:
			{
			}
			break;
			case ERotatorOverrideRotator::ConstantRotator:
			{
				rotator = mOverride.ConstantRotator;
			}
			break;
			case ERotatorOverrideRotator::LookActorTarget:
			{
				const TWeakObjectPtr<const AActor>* pp = mActorTargets.Find(mOverride.TargetName);
				if (!pp) break;
				if (!pp->IsValid()) break;
				FVector thisToTarget = (pp->Get()->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
				rotator = FRotationMatrix::MakeFromX(thisToTarget).Rotator();
			}
			break;
			case ERotatorOverrideRotator::LookLocationTarget:
			{
				const FVector* p = mLocationTargets.Find(mOverride.TargetName);
				if (!p) break;
				FVector thisToTarget = (*p - GetOwner()->GetActorLocation()).GetSafeNormal();
				rotator = FRotationMatrix::MakeFromX(thisToTarget).Rotator();
			}
			break;
			case ERotatorOverrideRotator::LookDirectionTarget:
			{
				const FVector* p = mDirectionTargets.Find(mOverride.TargetName);
				if (!p) break;
				rotator = mOverride.RotateDirection + FRotationMatrix::MakeFromX(*p).Rotator();
			}
			break;
		}
	}

	return rotator;
}

bool UCharacterRotateComponent::IsTarget(const FString& TargetName) const
{
	if (mActorTargets.Contains(TargetName)) return true;
	if (mLocationTargets.Contains(TargetName)) return true;
	if (mDirectionTargets.Contains(TargetName)) return true;
	return false;
}

bool UCharacterRotateComponent::AddActorTarget(const FString& TargetName, TWeakObjectPtr<const AActor> actor)
{
	if (IsTarget(TargetName)) return false;
	mActorTargets.Add(TargetName, actor);
	return true;
}

bool UCharacterRotateComponent::AddLocationTarget(const FString& TargetName, const FVector& location)
{
	if (IsTarget(TargetName)) return false;
	mLocationTargets.Add(TargetName, location);
	return true;
}

bool UCharacterRotateComponent::AddDirectionTarget(const FString& TargetName, const FVector& direction)
{
	if (IsTarget(TargetName)) return false;
	mDirectionTargets.Add(TargetName, direction);
	return true;
}

bool UCharacterRotateComponent::UpdateActorTarget(const FString& TargetName, TWeakObjectPtr<const AActor> actor)
{
	bool isTarget = IsTarget(TargetName);

	if (isTarget)
	{
		bool contains = mActorTargets.Contains(TargetName);

		if (contains)
		{
			mActorTargets[TargetName] = actor;
			return true;
		}
		else
		{
			// 다른 타입의 키입니다.
			return false;
		}
	}
	else
	{
		mActorTargets.Add(TargetName, actor);
		return true;
	}

	return false;
}

bool UCharacterRotateComponent::UpdateLocationTarget(const FString& TargetName, const FVector& location)
{
	bool isTarget = IsTarget(TargetName);

	if (isTarget)
	{
		bool contains = mLocationTargets.Contains(TargetName);

		if (contains)
		{
			mLocationTargets[TargetName] = location;
			return true;
		}
		else
		{
			// 다른 타입의 키입니다.
			return false;
		}
	}
	else
	{
		mLocationTargets.Add(TargetName, location);
		return true;
	}

	return false;
}

bool UCharacterRotateComponent::UpdateDirectionTarget(const FString& TargetName, const FVector& direction)
{
	bool isTarget = IsTarget(TargetName);

	if (isTarget)
	{
		bool contains = mDirectionTargets.Contains(TargetName);

		if (contains)
		{
			mDirectionTargets[TargetName] = direction;
			return true;
		}
		else
		{
			// 다른 타입의 키입니다.
			return false;
		}
	}
	else
	{
		mDirectionTargets.Add(TargetName, direction);
		return true;
	}

	return false;
}

bool UCharacterRotateComponent::RemoveTarget(const FString& TargetName)
{
	return (mActorTargets.Remove(TargetName) + mLocationTargets.Remove(TargetName) + mDirectionTargets.Remove(TargetName)) > 0;
}

void UCharacterRotateComponent::Rotate(float deltaTime)
{
	bool rotate = GetOverridedRotate();
	float speed = GetOverridedSpeed();
	FRotator rotator = GetOverridedRotator();

	if (rotate)
	{
		FRotator newRotation = FMath::RInterpTo(GetOwner()->GetActorRotation(), rotator, deltaTime, speed);

		if (mRotateOnXYCoordinates)
		{
			newRotation.Roll = 0.0f;
			newRotation.Pitch = 0.0f;
		}

		GetOwner()->SetActorRotation(newRotation);

		if (mOverride.Override && mOverride.OverrideRotate && mOverride.OverwriteRotate)
		{
			mRotator = rotator;
		}
	}
}

