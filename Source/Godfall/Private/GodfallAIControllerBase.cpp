// Fill out your copyright notice in the Description page of Project Settings.


#include "GodfallAIControllerBase.h"
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/BehaviorTree.h>
#include <Navigation/CrowdFollowingComponent.h>
#include "GodfallFollowingComponent.h"

//AGodfallAIControllerBase::AGodfallAIControllerBase(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer.SetDefaultSubobjectClass<UGodfallFollowingComponent>(TEXT("PathFollowingComponent")))
//{
//}

//AGodfallAIControllerBase::AGodfallAIControllerBase(const FObjectInitializer& ObjectInitializer) 
//	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
//{
//}

AGodfallAIControllerBase::AGodfallAIControllerBase(const FObjectInitializer& ObjectInitializer)
{
}

void AGodfallAIControllerBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AGodfallAIControllerBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGodfallAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AGodfallAIControllerBase::OnUnPossess()
{
	Super::OnUnPossess();
	StopAI();
}

void AGodfallAIControllerBase::SetAssets(UBehaviorTree* behaviorTreeAsset, UBlackboardData* blackboardAsset)
{
	if (!ensure(behaviorTreeAsset)) return;
	if (!ensure(blackboardAsset)) return;

	mBehaviorTreeAsset = behaviorTreeAsset;
	mBlackboardAsset = blackboardAsset;
}

void AGodfallAIControllerBase::RunAI()
{
	if (!ensure(mBehaviorTreeAsset)) return;
	if (!ensure(mBlackboardAsset)) return;

	UBlackboardComponent* blackboardComp = Blackboard.Get();
	if (UseBlackboard(mBlackboardAsset, blackboardComp))
	{
		ensure(RunBehaviorTree(mBehaviorTreeAsset));
	}
}

void AGodfallAIControllerBase::StopAI()
{
	UBehaviorTreeComponent* behaviorTreeComp = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (!behaviorTreeComp) return;
	behaviorTreeComp->StopTree(EBTStopMode::Safe);
}
