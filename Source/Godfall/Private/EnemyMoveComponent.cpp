// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMoveComponent.h"
#include "GodfallEnemyBase.h"
#include "GodfallEnemyAnimInstanceBase.h"
#include <NavigationPath.h>
#include "CharacterRotateComponent.h"

UEnemyMoveComponent::UEnemyMoveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UEnemyMoveComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UEnemyMoveComponent::BeginPlay()
{
	Super::BeginPlay();

	mEnemy = Cast<AGodfallEnemyBase>(GetOwner());
	if (ensure(mEnemy))
	{
		mEnemyAnimInstance = mEnemy->GetGodfallEnemyAnimInstance();
		ensure(mEnemyAnimInstance);
	}
}

void UEnemyMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!mEnemy || !mEnemyAnimInstance) return;
	if (!mMove) return;
	if (mMoveState == EEnemyMoveComponentState::Stop) return;

	if (IsNearWithTarget())
	{
		SetToSucceeded();
		return;
	}

	switch (mMoveState)
	{
		case EEnemyMoveComponentState::Move:
		{
			TickMove();
		}
		break;
		case EEnemyMoveComponentState::Wait:
		{
			TickWait(DeltaTime);
		}
		break;
		case EEnemyMoveComponentState::Avoid:
		{
			TickAvoid();
		}
		break;
	}
}

void UEnemyMoveComponent::Stop()
{
	SetToSucceeded();
}

void UEnemyMoveComponent::WalkToLocation(const FVector& targetLocation, E4Direction walkDirection)
{
	mMoveDistanceAccumulation = 0.0f;
	mPrevLocation = GetOwner()->GetActorLocation();

	mTargetLocation = targetLocation;
	mTargetType = EEnemyMoveComponentTargetType::Vector;
	mMoveType = EEnemyMoveType::Walk;
	mResult = EEnemyMoveComponentResult::InProgress;

	SetState(EEnemyMoveComponentState::Move);
}

void UEnemyMoveComponent::WalkToActor(const AActor* targetActor, E4Direction walkDirection)
{
	mMoveDistanceAccumulation = 0.0f;
	mPrevLocation = GetOwner()->GetActorLocation();

	mTargetActor = targetActor;
	mTargetType = EEnemyMoveComponentTargetType::Actor;
	mMoveType = EEnemyMoveType::Walk;
	mResult = EEnemyMoveComponentResult::InProgress;

	SetState(EEnemyMoveComponentState::Move);
}

void UEnemyMoveComponent::RunToLocation(const FVector& targetLocation)
{
	mTargetLocation = targetLocation;
	mTargetType = EEnemyMoveComponentTargetType::Vector;
	mMoveType = EEnemyMoveType::Run;
	mResult = EEnemyMoveComponentResult::InProgress;

	SetState(EEnemyMoveComponentState::Move);
}

void UEnemyMoveComponent::RunToActor(const AActor* targetActor)
{
	mTargetActor = targetActor;
	mTargetType = EEnemyMoveComponentTargetType::Actor;
	mMoveType = EEnemyMoveType::Run;
	mResult = EEnemyMoveComponentResult::InProgress;

	SetState(EEnemyMoveComponentState::Move);
}

void UEnemyMoveComponent::UptateTargetLocation(const FVector& targetLocation)
{
	mTargetLocation = targetLocation;
}

void UEnemyMoveComponent::UptateTargetActor(const AActor* targetActor)
{
	mTargetActor = targetActor;
}

bool UEnemyMoveComponent::GetTargetLocation(FVector& location) const
{
	switch (mTargetType)
	{
		case EEnemyMoveComponentTargetType::Vector:
		{
			location = mTargetLocation;
			return true;
		}
		break;
		case EEnemyMoveComponentTargetType::Actor:
		{
			if (!mTargetActor.IsValid()) return false;

			location = mTargetActor->GetActorLocation();
		}
		break;
	}

	return false;
}

bool UEnemyMoveComponent::IsNearWithTarget() const
{
	FVector targetLocation;
	if (!GetTargetLocation(targetLocation)) return true;

	switch (mTargetType)
	{
		case EEnemyMoveComponentTargetType::Actor:
		{
			const ACharacter* targetCharacter = Cast<const ACharacter>(mTargetActor);

			float thisRadius = mEnemy->GetCapsuleComponent()->GetScaledCapsuleRadius();
			float targetRadius = targetCharacter ? targetCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius() : 0.0f;

			float distanceWithTwoPoints = FVector::Distance(mEnemy->GetActorLocation(), targetLocation);
			float distanceWithTwoCapsules = FMath::Max(0.0f, distanceWithTwoPoints - (thisRadius + targetRadius));
			return distanceWithTwoCapsules <= mStopRadius;
		}
		break;
		case EEnemyMoveComponentTargetType::Vector:
		{
			return FVector::Distance(mEnemy->GetActorLocation(), targetLocation) <= mStopRadius;
		}
		break;
	}

	return true;
}

bool UEnemyMoveComponent::IsNearWith(const FVector& location, float distance) const
{
	return FVector::Distance(mEnemy->GetActorLocation(), location) <= distance;
}

bool UEnemyMoveComponent::IsNearWith(const AActor* actor, float distance) const
{
	const ACharacter* targetCharacter = Cast<const ACharacter>(mTargetActor);

	float thisRadius = mEnemy->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float targetRadius = targetCharacter ? targetCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius() : 0.0f;

	float distanceWithTwoPoints = FVector::Distance(mEnemy->GetActorLocation(), actor->GetActorLocation());
	float distanceWithTwoCapsules = FMath::Max(0.0f, distanceWithTwoPoints - (thisRadius + targetRadius));
	return distanceWithTwoCapsules <= distance;
}

bool UEnemyMoveComponent::GetNextLocation(FVector& nextLocation, bool* isPartial) const
{
	UNavigationPath* path = nullptr;
	FVector targetLocation;
	if (GetTargetLocation(targetLocation))
	{
		path = UNavigationSystemV1::FindPathToLocationSynchronously(
			GetWorld(),
			mEnemy->GetActorLocation(),
			targetLocation,
			mEnemy
		);
	}

	if (isPartial)
	{
		*isPartial = path ? path->IsPartial() : false;
	}

	if (path && path->PathPoints.Num() > 1)
	{
		nextLocation = path->PathPoints[1];
		return true;
	}

	return false;
}

bool UEnemyMoveComponent::GetNextAvoidLocation(FVector& nextLocation, bool* isPartial) const
{
	UNavigationPath* path = nullptr;
	path = UNavigationSystemV1::FindPathToLocationSynchronously(
		GetWorld(),
		mEnemy->GetActorLocation(),
		mAvoidTargetLocation,
		mEnemy
	);

	if (isPartial)
	{
		*isPartial = path ? path->IsPartial() : false;
	}

	if (path && path->PathPoints.Num() > 1)
	{
		nextLocation = path->PathPoints[1];
		return true;
	}

	return false;
}

FVector UEnemyMoveComponent::RotateVector(const FVector& vector, E4Direction rotateDirection) const
{
	FRotator rotator;
	switch (rotateDirection)
	{
		case E4Direction::Forward: rotator = FRotator::ZeroRotator; break;
		case E4Direction::Backward: rotator = FRotator(0.0f, 180.0f, 0.0f); break;
		case E4Direction::Right: rotator = FRotator(0.0f, 90.0f, 0.0f); break;
		case E4Direction::Left: rotator = FRotator(0.0f, -90.0f, 0.0f); break;
	}
	return rotator.RotateVector(vector);
}

bool UEnemyMoveComponent::IsBlocked(const FVector& nextDirection, E4Direction rotateDirection, float distance, FName tagName, FName profileName, float& hitDistance) const
{
	if (!mEnemy) return false;

	FVector sweepDirection = RotateVector(nextDirection, rotateDirection);

	UCapsuleComponent* capsuleComp = mEnemy->GetCapsuleComponent();
	FCollisionShape capsuleShape = capsuleComp->GetCollisionShape();
	FVector capsuleLocation = capsuleComp->GetComponentLocation();
	FRotator capsuleRotation = capsuleComp->GetComponentRotation();
	FCollisionQueryParams params(tagName, false, mEnemy);

	if(mAvoidCollisionIgnore.IsValid()) params.AddIgnoredActor(mAvoidCollisionIgnore.Get());
	params.AddIgnoredActors(mAvoidCollisionIgnores);
	FHitResult hit;
	
	bool sweep = GetWorld()->SweepSingleByProfile(
		hit,
		capsuleLocation,
		capsuleLocation + sweepDirection * distance,
		capsuleRotation.Quaternion(),
		profileName,
		capsuleShape,
		params);

	hitDistance = hit.Distance;

	return sweep;
}

bool UEnemyMoveComponent::IsBlocked(const FVector& nextDirection, E4Direction rotateDirection, float distance, FName tagName, FName profileName) const
{
	float hitDistance;
	return IsBlocked(nextDirection, rotateDirection, distance, tagName, profileName, hitDistance);
}

bool UEnemyMoveComponent::IsBlockedByOtherCharacter(const FVector& nextDirection, E4Direction rotateDirection, float distance) const
{
	return IsBlocked(nextDirection, rotateDirection, distance, GodfallTags::Enemy, GodfallPresets::SweepPawn);
}

bool UEnemyMoveComponent::IsBlockedByOtherCharacterOrStatic(const FVector& nextDirection, E4Direction rotateDirection, float distance) const
{
	if (IsBlocked(nextDirection, rotateDirection, distance, GodfallTags::Enemy, GodfallPresets::SweepPawn)) return true;
	//if (IsBlocked(nextDirection, rotateDirection, distance, NAME_None, GodfallPresets::SweepStatic)) return true;
	return false;
}

void UEnemyMoveComponent::SetState(EEnemyMoveComponentState state)
{
	mMoveState = state;

	switch (state)
	{
		case EEnemyMoveComponentState::Stop:
		{
			mMoveState = EEnemyMoveComponentState::Stop;
			ApplyStopToAnimInstance();
		}
		break;
		case EEnemyMoveComponentState::Move:
		{
			ApplyMoveTypeToAnimInstance(mMoveType);

			// Move 상태로 변경될 때 쳐다보는 방향을 변경합니다.
			if (mLookOnChangedMoveState)
			{
				FVector nextLocation;
				FVector nextDirection;
				bool isPartial;
				if (GetNextLocation(nextLocation, &isPartial))
				{
					nextDirection = GetActorToTargetXYDirection(nextLocation);
					SetLookDirection(nextDirection);
				}

				if (mStopIfPartialPath && isPartial)
				{
					SetToFailed();
				}
			}
		}
		break;
		case EEnemyMoveComponentState::Wait:
		{
			mWaitAccumulation = 0.0f;
			ApplyStopToAnimInstance();
		}
		break;
		case EEnemyMoveComponentState::Avoid:
		{
			bool B = IsBlockedByOtherCharacterOrStatic(GetOwner()->GetActorForwardVector(), E4Direction::Backward, mAvoidDistance);
			bool R = IsBlockedByOtherCharacterOrStatic(GetOwner()->GetActorForwardVector(), E4Direction::Right, mAvoidDistance);
			bool L = IsBlockedByOtherCharacterOrStatic(GetOwner()->GetActorForwardVector(), E4Direction::Left, mAvoidDistance);
			bool rightPriority = bool(FMath::Rand() % 2);

			mAvoidTargetLocation = GetOwner()->GetActorLocation();

			if (!R || !L)
			{
				if (rightPriority)
				{
					if (!R)
					{
						mAvoidTargetLocation += RotateVector(GetOwner()->GetActorForwardVector(), E4Direction::Right) * mAvoidDistance;
						mAvoidDirection = E4Direction::Right;
						ApplyWalkDirectionToAnimInstance(E4Direction::Right);
					}
					else if (!L)
					{
						mAvoidTargetLocation += RotateVector(GetOwner()->GetActorForwardVector(), E4Direction::Left) * mAvoidDistance;
						mAvoidDirection = E4Direction::Left;
						ApplyWalkDirectionToAnimInstance(E4Direction::Left);
					}
				}
				else
				{
					if (!L)
					{
						mAvoidTargetLocation += RotateVector(GetOwner()->GetActorForwardVector(), E4Direction::Left) * mAvoidDistance;
						mAvoidDirection = E4Direction::Left;
						ApplyWalkDirectionToAnimInstance(E4Direction::Left);
					}
					else if (!R)
					{
						mAvoidTargetLocation += RotateVector(GetOwner()->GetActorForwardVector(), E4Direction::Right) * mAvoidDistance;
						mAvoidDirection = E4Direction::Right;
						ApplyWalkDirectionToAnimInstance(E4Direction::Right);
					}
				}
			}
			else
			{
				mAvoidTargetLocation += RotateVector(GetOwner()->GetActorForwardVector(), E4Direction::Backward) * mAvoidDistance;
				mAvoidDirection = E4Direction::Backward;
				ApplyWalkDirectionToAnimInstance(E4Direction::Backward);
			}

			ApplyMoveTypeToAnimInstance(EEnemyMoveType::Walk);
		}
		break;
	}
}

void UEnemyMoveComponent::TickMove()
{
	AccumulateMoveDistance();

	FVector nextLocation;
	FVector nextDirection;
	bool isPartial;
	if (GetNextLocation(nextLocation, &isPartial))
	{
		nextDirection = GetActorToTargetXYDirection(nextLocation);
		SetLookDirection(nextDirection);

		if (isPartial && mStopIfPartialPath)
		{
			SetToFailed();
		}
	}
	else
	{
		SetToFailed();
	}

	if (mUseWait)
	{
		bool blockedByOtherChracter = IsBlockedByOtherCharacter(nextDirection, E4Direction::Forward, mBlockDistance);
		if (blockedByOtherChracter)
		{
			SetState(EEnemyMoveComponentState::Wait);
		}
	}
}

void UEnemyMoveComponent::TickWait(float deltaTime)
{
	mWaitAccumulation += deltaTime;
	
	if (mUseAvoid)
	{
		if (CanAvoid())
		{
			SetState(EEnemyMoveComponentState::Avoid);
			return;
		}
	}

	FVector nextLocation;
	FVector nextDirection;
	if (GetNextLocation(nextLocation))
	{
		nextDirection = GetActorToTargetXYDirection(nextLocation);
		
		// Wait 상태일 때 쳐다보는 방향을 변경합니다.
		if (mLookOnWait)
		{
			SetLookDirection(nextDirection);
		}
	}
	else
	{
		return;
	}

	bool blockedByOtherChracter = IsBlockedByOtherCharacter(nextDirection, E4Direction::Forward, mBlockDistance);
	if (!blockedByOtherChracter)
	{
		SetState(EEnemyMoveComponentState::Move);
	}
}

bool UEnemyMoveComponent::CanAvoid() const
{
	FVector nextLocation;
	FVector nextDirection;
	if (GetNextLocation(nextLocation))
	{
		nextDirection = GetActorToTargetXYDirection(nextLocation);
	}
	else
	{
		nextDirection = GetOwner()->GetActorForwardVector();
	}

	bool B = IsBlockedByOtherCharacterOrStatic(nextDirection, E4Direction::Backward, mAvoidDistance);
	bool R = IsBlockedByOtherCharacterOrStatic(nextDirection, E4Direction::Right, mAvoidDistance);
	bool L = IsBlockedByOtherCharacterOrStatic(nextDirection, E4Direction::Left, mAvoidDistance);

	return ((B + R + L) < 3);
}

bool UEnemyMoveComponent::IsNearWithAvoidTarget() const
{
	float thisRadius = mEnemy->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float distanceWithTwoPoints = FVector::Distance(mEnemy->GetActorLocation(), mAvoidTargetLocation);
	float distanceWithTwoCapsules = FMath::Max(0.0f, distanceWithTwoPoints - (thisRadius));
	return distanceWithTwoCapsules <= mStopRadius;
}

void UEnemyMoveComponent::TickAvoid()
{
	AccumulateMoveDistance();

	if (IsNearWithAvoidTarget())
	{
		SetState(EEnemyMoveComponentState::Move);
		return;
	}

	FVector nextLocation;
	FVector nextDirection;
	if (GetNextAvoidLocation(nextLocation))
	{
		nextDirection = GetActorToTargetXYDirection(nextLocation);
		SetLookDirection(nextDirection);
	}
	else
	{
		SetState(EEnemyMoveComponentState::Move);
		return;
	}

	bool blockedByOtherChracter = IsBlockedByOtherCharacterOrStatic(GetOwner()->GetActorForwardVector(), mAvoidDirection, mBlockDistance);
	if (blockedByOtherChracter)
	{
		SetState(EEnemyMoveComponentState::Move);
	}
}

void UEnemyMoveComponent::SetToSucceeded()
{
	mResult = EEnemyMoveComponentResult::Succeeded;

	SetState(EEnemyMoveComponentState::Stop);
}

void UEnemyMoveComponent::SetToFailed()
{
	mResult = EEnemyMoveComponentResult::Failed;

	SetState(EEnemyMoveComponentState::Stop);
}

void UEnemyMoveComponent::SetLookDirection(const FVector& direction)
{
	if (!mEnemy) return;

	FRotator rotation = FRotationMatrix::MakeFromX(direction).Rotator();

	if (mMoveType == EEnemyMoveType::Walk)
	{
		switch (mWalkDirection)
		{
			case E4Direction::Forward:
				break;
			case E4Direction::Backward:
				rotation.Add(0.0f, 180.0f, 0.0f);
				break;
			case E4Direction::Left:
				rotation.Add(0.0f, 90.0f, 0.0f);
				break;
			case E4Direction::Right:
				rotation.Add(0.0f, -90.0f, 0.0f);
				break;
		}

		ApplyWalkDirectionToAnimInstance(mWalkDirection);
	}
	else if (mMoveState == EEnemyMoveComponentState::Avoid)
	{
		switch (mAvoidDirection)
		{
			case E4Direction::Forward:
				break;
			case E4Direction::Backward:
				rotation.Add(0.0f, 180.0f, 0.0f);
				break;
			case E4Direction::Left:
				rotation.Add(0.0f, 90.0f, 0.0f);
				break;
			case E4Direction::Right:
				rotation.Add(0.0f, -90.0f, 0.0f);
				break;
		}

		ApplyWalkDirectionToAnimInstance(mAvoidDirection);
	}

	mEnemy->GetCharacterRotateComponent()->SetRotator(rotation);
}

void UEnemyMoveComponent::ApplyStopToAnimInstance()
{
	if (!mEnemyAnimInstance) return;

	mEnemyAnimInstance->SetMoveSpeed(EEnemyMoveSpeed::None);
}

void UEnemyMoveComponent::ApplyMoveTypeToAnimInstance(EEnemyMoveType moveType)
{
	if (!mEnemyAnimInstance) return;

	switch (moveType)
	{
		case EEnemyMoveType::Walk:
			mEnemyAnimInstance->SetMoveSpeed(EEnemyMoveSpeed::Walk);
			break;
		case EEnemyMoveType::Run:
			mEnemyAnimInstance->SetMoveSpeed(EEnemyMoveSpeed::Run);
			break;
	}
}

void UEnemyMoveComponent::ApplyWalkDirectionToAnimInstance(E4Direction direction)
{
	if (!mEnemyAnimInstance) return;

	mEnemyAnimInstance->SetWalkDirection(direction);
}

FVector UEnemyMoveComponent::ConvertXYDirection(const FVector& direction) const
{
	FVector newDirection = FVector::VectorPlaneProject(direction, FVector::UpVector);
	newDirection.Normalize();
	return newDirection;
}

FVector UEnemyMoveComponent::GetActorToTargetXYDirection(const FVector& targetLocation) const
{
	FVector actorToTarget = targetLocation - GetOwner()->GetActorLocation();
	return ConvertXYDirection(actorToTarget);
}

void UEnemyMoveComponent::AccumulateMoveDistance()
{
	FVector currentLocation = GetOwner()->GetActorLocation();
	float delta = FVector::Distance(currentLocation, mPrevLocation);
	mMoveDistanceAccumulation += delta;
	mPrevLocation = currentLocation;
}

