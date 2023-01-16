// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallCharacterBase.h"
#include "GodfallEnemyEnums.h"
#include "GodfallAIControllerBase.h"
#include "StateTable.h"
#include "GodfallEnemyBase.generated.h"

/**
 * 
 */
UCLASS()
class GODFALL_API AGodfallEnemyBase : public AGodfallCharacterBase
{
	GENERATED_BODY()
	
public:
	AGodfallEnemyBase();

protected:
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void PossessedByAIController(AAIController* NewAIController) {}
	virtual void PossessedByGodfallAIController(AGodfallAIControllerBase* NewGodfallAIController) {}
	virtual void SetupStateTable(FStateTable& mStateTable) {}
	virtual void UnPossessed() override;
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void OnDestroyedCallback(AActor* destroyed) override;

	virtual bool CanRotate() const override;
	virtual bool IsInvincible() const override;

	virtual void OnBeginExecution();
	virtual void OnEndExecution();
	virtual void OnDead() override;

	virtual void OnHealthChanged(float prev, float changed) override;
	virtual void OnSteminaChanged(float prev, float changed) override;
	void UpdateFloatingUI();

public:
	void InitAI(FString behaviorTreeAssetReferencePath, FString blackboardAssetReferencePath, UClass* aiControllerClass = AGodfallAIControllerBase::StaticClass());

	bool IsExecutableEnemy() const; // Return true if it has Execute montage
	float GetExecutionDamage() const { return mExecutionDamage; }

	class AActor* GetEnemyTargetActor() const;

	class UGodfallEnemyAnimInstanceBase* GetGodfallEnemyAnimInstance() const { return mGodfallEnemyAnimInstance; }
	class UEnemyMoveComponent* GetEnemyMoveComponent() const { return EnemyMoveComponent; }
	class AGodfallAIControllerBase* GetEnemyControllerBase() const { return mGodfallAIControllerBase; }

private:
	UPROPERTY()
	class UGodfallEnemyAnimInstanceBase* mGodfallEnemyAnimInstance = nullptr;

	UPROPERTY()
	class AGodfallAIControllerBase* mGodfallAIControllerBase = nullptr;

	UPROPERTY(EditAnywhere, DisplayName = "Behavior tree asset", Category = "Actor", Meta = (AllowPrivateAccess))
	UBehaviorTree* mBehaviorTreeAsset = nullptr;

	UPROPERTY(EditAnywhere, DisplayName = "Blackboard asset", Category = "Actor", Meta = (AllowPrivateAccess))
	UBlackboardData* mBlackboardAsset = nullptr;

protected:
	UPROPERTY(VisibleAnywhere, DisplayName = "Enemy Move", Category = "Actor", Meta = (AllowPrivateAccess))
	class UEnemyMoveComponent* EnemyMoveComponent = nullptr;

	UPROPERTY(VisibleAnywhere, DisplayName = "Floating Character UI", Category = "Actor", Meta = (AllowPrivateAccess))
	class UWidgetComponent* FloatingCharacterUIComponent = nullptr;

	UPROPERTY(EditAnywhere, DisplayName = "Execution Damage", Category = "Actor", Meta = (AllowPrivateAccess))
	float mExecutionDamage = 100.0f;

public:
	UPROPERTY(EditAnywhere, DisplayName = "Floating UI Height", Category = "Actor", Meta = (AllowPrivateAccess))
	float mFloatingUIHeight = 100.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Initial UI Attach Bone Name", Category = "Actor", Meta = (AllowPrivateAccess))
	FName mInitialUIAttachBoneName = TEXT("head");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Interp, DisplayName = "Run AI", Category = "Actor", Meta = (AllowPrivateAccess))
	bool mRunAI = true;
};
