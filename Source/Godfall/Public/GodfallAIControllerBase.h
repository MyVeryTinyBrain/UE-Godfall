// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "AIController.h"
#include "GodfallAIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class GODFALL_API AGodfallAIControllerBase : public AAIController
{
	GENERATED_BODY()
	
public:
	AGodfallAIControllerBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

public:
	void SetAssets(UBehaviorTree* behaviorTreeAsset, UBlackboardData* blackboardAsset);
	void RunAI();
	void StopAI();

protected:
	UPROPERTY()
	UBehaviorTree* mBehaviorTreeAsset = nullptr;

	UPROPERTY()
	UBlackboardData* mBlackboardAsset = nullptr;
};
