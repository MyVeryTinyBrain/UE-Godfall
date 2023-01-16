// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "Components/ActorComponent.h"
#include "GodfallEnemyEnums.h"
#include "EnemyMoveComponent.generated.h"

UENUM(BlueprintType)
enum class EEnemyMoveComponentTargetType : uint8
{
	Vector,
	Actor,
};

UENUM(BlueprintType)
enum class EEnemyMoveComponentState : uint8
{
	Stop,
	Move,
	Wait,
	Avoid,
};

UENUM(BlueprintType)
enum class EEnemyMoveComponentResult : uint8
{
	InProgress,
	Succeeded,
	Failed,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GODFALL_API UEnemyMoveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnemyMoveComponent();

protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void Stop();
	void WalkToLocation(const FVector& targetLocation, E4Direction walkDirection);
	void WalkToActor(const AActor* targetActor, E4Direction walkDirection);
	void RunToLocation(const FVector& targetLocation);
	void RunToActor(const AActor* targetActor);
	void UptateTargetLocation(const FVector& targetLocation);
	void UptateTargetActor(const AActor* targetActor);
	EEnemyMoveComponentState GetMoveState() const { return mMoveState; }
	EEnemyMoveComponentResult GetResult() const { return mResult; }

	bool GetTargetLocation(FVector& location) const;
	bool IsNearWithTarget() const;
	bool IsNearWith(const FVector& location, float distance) const;
	bool IsNearWith(const AActor* actor, float distance) const;
	bool GetNextLocation(FVector& nextLocation, bool* isPartial = nullptr) const;
	bool GetNextAvoidLocation(FVector& nextLocation, bool* isPartial = nullptr) const;
	FVector RotateVector(const FVector& vector, E4Direction rotateDirection) const;
	bool IsBlocked(const FVector& nextDirection, E4Direction rotateDirection, float distance, FName tagName, FName profileName, float& hitDistance) const;
	bool IsBlocked(const FVector& nextDirection, E4Direction rotateDirection, float distance, FName tagName, FName profileName) const;
	bool IsBlockedByOtherCharacter(const FVector& nextDirection, E4Direction rotateDirection, float distance) const;
	bool IsBlockedByOtherCharacterOrStatic(const FVector& nextDirection, E4Direction rotateDirection, float distance) const;

	float GetMoveDistanceAccumulation() const { return mMoveDistanceAccumulation; }

private:
	void SetState(EEnemyMoveComponentState state);
	void TickMove();
	void TickWait(float deltaTime);
	bool CanAvoid() const;
	bool IsNearWithAvoidTarget() const;
	void TickAvoid();

	void SetToSucceeded();
	void SetToFailed();
	void SetLookDirection(const FVector& direction);
	void ApplyStopToAnimInstance();
	void ApplyMoveTypeToAnimInstance(EEnemyMoveType moveType);
	void ApplyWalkDirectionToAnimInstance(E4Direction direction);
	FVector ConvertXYDirection(const FVector& direction) const;
	FVector GetActorToTargetXYDirection(const FVector& targetLocation) const;

	void AccumulateMoveDistance();

public:
	/* 이 컴포넌트를 통한 이동 허용 여부입니다. */
	UPROPERTY(EditAnywhere, DisplayName = "Move")
	bool mMove = true;

	UPROPERTY(EditAnywhere, DisplayName = "Stop if partial path")
	/* 목표 지점에 도달할 수 없고 목표에 가장 가까워지는 경우를 Partial 이라고 합니다.
	이 경우에 그나마 최선의 위치로 이동할지, 아니면 멈출지에 대한 플래그입니다.
	활성화하면 도달할 수 없을 때, 멈춥니다.*/
	bool mStopIfPartialPath = true;

	UPROPERTY(EditAnywhere, DisplayName = "Look On Changed Move State")
	/* Move State가 되었을 때 목표로 향하는 방향을 쳐다봅니다.
	Move State중에도 목표로 향하는 방향을 쳐다보지만,
	목표와의 거리가 가까울 때의 이동 명령을 이동하지 않고 종료되어 목표 지점을 쳐다보지 않게 됩니다.
	무조건 목표 지점을 쳐다보게 하기 위해서 사용되는 플래그입니다. */
	bool mLookOnChangedMoveState = true;

	/* 이 컴포넌트의 소유자가 가진 캡슐로 이동 방향 혹은 이동하려는 방향으로 스윕하여 멈추거나 회피하는데 사용되는 거리입니다. */
	UPROPERTY(EditAnywhere, DisplayName = "Block Distance", Meta = (ClampMin = 0.1f))
	float mBlockDistance = 100.0f;

	/* 목표까지의 거리가 이것보다 작다면 멈추게 됩니다.
	캐릭터가 목표인 경우에는 이 캐릭터와 목표 캐릭터의 반지름을 합친 것만큼의 거리를 점과 점 사이의 거리에서 제외하여 연산합니다.
	벡터가 목표인 경우에는 이 캐릭터의 반지름을 점과 점 사이의 거리에서 제외하여 연산합니다.
	이것은 캐릭터의 캡슐 반지름으로 인해 목표에 영원히 다다를 수 없을 경우를 방지하기 위함입니다. */
	UPROPERTY(EditAnywhere, DisplayName = "Stop Radius", Meta = (ClampMin = 0.1f))
	float mStopRadius = 100.0f;

	/* 이동하고자 하는 방향이 다른 캐릭터나 콜라이더에 의해 막힌 경우에 멈춥니다. */
	UPROPERTY(EditAnywhere, DisplayName = "Use Wait")
	bool mUseWait = true;

	/* 멈추는 중에 목표를 쳐다봅니다. */
	UPROPERTY(EditAnywhere, DisplayName = "Look On Wait", Meta = (EditCondition = "mUseWait == true", EditConditionHides))
	bool mLookOnWait = true;

	/* 멈추는 중에 회피 상태로 전환할 수 있습니다. */
	UPROPERTY(EditAnywhere, DisplayName = "Use Avoid")
	bool mUseAvoid = true;

	/* 회피를 시작했을때의 거리로부터 얼마나 멀리 회피할지에 대한 거리입니다. */
	UPROPERTY(EditAnywhere, DisplayName = "Avoid Distance", Meta = (EditCondition = "mUseAvoid == true", EditConditionHides, ClampMin = 0.1f))
	float mAvoidDistance = 450.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Move Type")
	EEnemyMoveType mMoveType = EEnemyMoveType::Run;

	UPROPERTY(EditAnywhere, DisplayName = "Walk Direction", Meta = (EditCondition = "mMoveType == EEnemyMoveType::Walk", EditConditionHides))
	E4Direction mWalkDirection = E4Direction::Forward;

	UPROPERTY(EditAnywhere, DisplayName = "Target Type")
	EEnemyMoveComponentTargetType mTargetType = EEnemyMoveComponentTargetType::Vector;

	UPROPERTY(EditAnywhere, DisplayName = "Target Location", Meta = (EditCondition = "mTargetType == EEnemyMoveComponentTargetType::Vector", EditConditionHides))
	FVector mTargetLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, DisplayName = "Target Actor", Meta = (EditCondition = "mTargetType == EEnemyMoveComponentTargetType::Actor", EditConditionHides))
	TWeakObjectPtr<const AActor> mTargetActor = nullptr;

	UPROPERTY(EditAnywhere, DisplayName = "Avoid Collision Ignore", Meta = (EditCondition = "mUseAvoid == true", EditConditionHides))
	TWeakObjectPtr<const AActor> mAvoidCollisionIgnore;

	UPROPERTY(EditAnywhere, DisplayName = "Avoid Collision Ignores", Meta = (EditCondition = "mUseAvoid == true", EditConditionHides))
	TArray<TWeakObjectPtr<const AActor>> mAvoidCollisionIgnores;

private:
	UPROPERTY()
	class AGodfallEnemyBase* mEnemy = 0;

	UPROPERTY()
	class UGodfallEnemyAnimInstanceBase* mEnemyAnimInstance = 0;

	UPROPERTY(VisibleAnywhere, DisplayName = "Move State", Meta = (AllowPrivateAccess))
	EEnemyMoveComponentState mMoveState = EEnemyMoveComponentState::Stop;

	UPROPERTY(VisibleAnywhere, DisplayName = "Move Result", Meta = (AllowPrivateAccess))
	EEnemyMoveComponentResult mResult = EEnemyMoveComponentResult::Succeeded;

	UPROPERTY(VisibleAnywhere, DisplayName = "Wait Accumulation", Meta = (AllowPrivateAccess))
	float mWaitAccumulation = 0.0f;

	UPROPERTY(VisibleAnywhere, DisplayName = "Avoid Target Location", Meta = (AllowPrivateAccess))
	FVector mAvoidTargetLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, DisplayName = "Avoid Direction", Meta = (AllowPrivateAccess))
	E4Direction mAvoidDirection = E4Direction::Backward;

	UPROPERTY()
	FVector mPrevLocation;

	UPROPERTY()
	float mMoveDistanceAccumulation = 0.0f;
};
