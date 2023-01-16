// Fill out your copyright notice in the Description page of Project Settings.


#include "GodfallEnemyBase.h"
#include "GodfallAIControllerBase.h"
#include "GodfallEnemyAnimInstanceBase.h"
#include "CharacterRotateComponent.h"
#include "EnemyMoveComponent.h"
#include <BehaviorTree/BlackboardComponent.h>
#include "BlackboardKeyType_StateTable.h"
#include "GodfallGameState.h"
#include "CharacterManagerComponent.h"
#include <Components/WidgetComponent.h>
#include "FloatingCharacterUI.h"
#include "GodfallBlackboardKeys.h"

AGodfallEnemyBase::AGodfallEnemyBase()
{
	Tags.Add(GodfallTags::Enemy);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	EnemyMoveComponent = CreateDefaultSubobject<UEnemyMoveComponent>(TEXT("Enemy Move Component"));

	FloatingCharacterUIComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Floating UI"));
	FloatingCharacterUIComponent->SetupAttachment(RootComponent);
	FloatingCharacterUIComponent->SetWidgetSpace(EWidgetSpace::Screen);
	FloatingCharacterUIComponent->SetDrawSize(FVector2D(200.0f, 30.0f));

	ConstructorHelpers::FClassFinder<UFloatingCharacterUI> floatingUI(TEXT("/Game/UI/EnemyFloatingUI.EnemyFloatingUI_C"));
	if (ensure(floatingUI.Succeeded()))
	{
		FloatingCharacterUIComponent->SetWidgetClass(floatingUI.Class);
	}
}

void AGodfallEnemyBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	USkeletalMeshComponent* characterMesh = GetMesh();
	if (ensure(characterMesh))
	{
		UAnimInstance* animInstance = characterMesh->GetAnimInstance();
		mGodfallEnemyAnimInstance = Cast<UGodfallEnemyAnimInstanceBase>(animInstance);
	}
}

void AGodfallEnemyBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AAIController* aiController = Cast<AAIController>(NewController);
	mGodfallAIControllerBase = Cast<AGodfallAIControllerBase>(NewController);
	if (mGodfallAIControllerBase)
	{
		if (ensure(mBehaviorTreeAsset) && 
			ensure(mBlackboardAsset))
		{
			mGodfallAIControllerBase->SetAssets(mBehaviorTreeAsset, mBlackboardAsset);
			mGodfallAIControllerBase->RunAI();
		}
	}

	if (aiController)
	{
		PossessedByAIController(aiController);

		FStateTable mStateTable;
		SetupStateTable(mStateTable);

		UBlackboardComponent* blackboardComp = aiController->GetBlackboardComponent();
		if (ensure(blackboardComp))
		{
			blackboardComp->SetValue<UBlackboardKeyType_StateTable>(GodfallBlackboardKeys::StateTable, mStateTable);
			blackboardComp->SetValueAsObject(GodfallBlackboardKeys::SelfEnemy, this);
		}
	}
	if (mGodfallAIControllerBase)
	{
		PossessedByGodfallAIController(mGodfallAIControllerBase);
	}
}

void AGodfallEnemyBase::UnPossessed()
{
	Super::UnPossessed();
}

void AGodfallEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterRotateComponent()->AddActorTarget(TEXT("Target"), GetWorld()->GetFirstPlayerController()->GetPawn());

	if (mGodfallEnemyAnimInstance)
	{
		mGodfallEnemyAnimInstance->OnBeginExecution.AddUObject(this, &AGodfallEnemyBase::OnBeginExecution);
		mGodfallEnemyAnimInstance->OnEndExecution.AddUObject(this, &AGodfallEnemyBase::OnEndExecution);
	}

	if (GetMesh() && INDEX_NONE != GetMesh()->GetBoneIndex(mInitialUIAttachBoneName))
	{
		FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
		FloatingCharacterUIComponent->AttachToComponent(GetMesh(), rules, mInitialUIAttachBoneName);
	}
	FloatingCharacterUIComponent->SetHiddenInGame(true);
	//UpdateFloatingUI();
}
#include <BrainComponent.h>
void AGodfallEnemyBase::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	UBlackboardComponent* blackboardComp = mGodfallAIControllerBase ? mGodfallAIControllerBase->GetBlackboardComponent() : nullptr;
	if (ensure(blackboardComp))
	{
		blackboardComp->SetValueAsBool(GodfallBlackboardKeys::Exitable, mGodfallEnemyAnimInstance->IsExitable());
		blackboardComp->SetValueAsBool(GodfallBlackboardKeys::Transitable, mGodfallEnemyAnimInstance->IsTransitable());

		UBrainComponent* brainComp = blackboardComp->GetBrainComponent();
		if (brainComp)
		{
			if (mRunAI)
			{
				if (brainComp->IsPaused())
				{
					brainComp->RestartLogic();
					brainComp->ResumeLogic(TEXT(""));
				}
			}
			else
			{
				if (!brainComp->IsPaused())
				{
					brainComp->PauseLogic(TEXT(""));
				}
			}
		}
	}

	if (ensure(FloatingCharacterUIComponent && FloatingCharacterUIComponent->GetAttachParent()))
	{
		FVector uiLocation = FloatingCharacterUIComponent->GetAttachParent()->GetComponentLocation() + mFloatingUIHeight * FVector::UpVector;
		USkeletalMeshComponent* mesh = Cast<USkeletalMeshComponent>(FloatingCharacterUIComponent->GetAttachParent());
		if (mesh && INDEX_NONE != mesh->GetBoneIndex(mInitialUIAttachBoneName))
		{
			uiLocation = mesh->GetBoneLocation(mInitialUIAttachBoneName) + mFloatingUIHeight * FVector::UpVector;
		}
		FloatingCharacterUIComponent->SetWorldLocation(uiLocation);
	}
}

void AGodfallEnemyBase::OnDestroyedCallback(AActor* destroyed)
{
	Super::OnDestroyedCallback(destroyed);

	AGodfallAIControllerBase* godfallAIController = Cast<AGodfallAIControllerBase>(GetController());
	if (godfallAIController)
	{
		if (ensure(mBehaviorTreeAsset) &&
			ensure(mBlackboardAsset))
		{
			godfallAIController->StopAI();
		}
	}
}

bool AGodfallEnemyBase::CanRotate() const
{
	return !mGodfallAnimInstance->IsAnyMontagePlaying();
}

bool AGodfallEnemyBase::IsInvincible() const
{
	if (Super::IsInvincible()) return true;
	return mGodfallEnemyAnimInstance->IsActiveExecutionMontage();
}

void AGodfallEnemyBase::OnBeginExecution()
{
	AGodfallGameState* gameState = Cast<AGodfallGameState>(GetWorld()->GetGameState());
	if (ensure(gameState))
	{
		auto enemies = gameState->GetCharacterManager()->GetEnemies();
		for (auto& enemy : enemies)
		{
			if (!enemy.IsValid()) continue;
			if (enemy == this) continue;
			MoveIgnoreActorAdd(enemy.Get());
		}
	}

	bool temp;
	SetHealth(GetHealth() - mExecutionDamage, temp);
	SetStemina(0.0f);
}

void AGodfallEnemyBase::OnEndExecution()
{
	AGodfallGameState* gameState = Cast<AGodfallGameState>(GetWorld()->GetGameState());
	if (ensure(gameState))
	{
		auto enemies = gameState->GetCharacterManager()->GetEnemies();
		for (auto& enemy : enemies)
		{
			if (!enemy.IsValid()) continue;
			if (enemy == this) continue;
			MoveIgnoreActorRemove(enemy.Get());
		}
	}

	bool isZeroHealth = GetHealth() <= 0.0f;
	if (isZeroHealth)
	{
		ToDead();
	}
}

void AGodfallEnemyBase::OnDead()
{
	Super::OnDead();

	AGodfallAIControllerBase* godfallAIController = Cast<AGodfallAIControllerBase>(GetController());
	if (godfallAIController)
	{
		godfallAIController->StopAI();
	}
}

void AGodfallEnemyBase::OnHealthChanged(float prev, float changed)
{
	UpdateFloatingUI();
}

void AGodfallEnemyBase::OnSteminaChanged(float prev, float changed)
{
	UpdateFloatingUI();
}

void AGodfallEnemyBase::UpdateFloatingUI()
{
	if (ensure(FloatingCharacterUIComponent))
	{
		UFloatingCharacterUI* widget = Cast<UFloatingCharacterUI>(FloatingCharacterUIComponent->GetWidget());

		float healthPercent = GetHealth() / GetMaxHealth();

		FloatingCharacterUIComponent->SetHiddenInGame(healthPercent <= 0.0f);

		if (ensure(widget))
		{
			widget->SetHealthPercent(healthPercent);
			widget->SetSteminaPercent(GetStemina() / GetMaxStemina());
		}
	}
}

void AGodfallEnemyBase::InitAI(FString behaviorTreeAssetReferencePath, FString blackboardAssetReferencePath, UClass* aiControllerClass)
{
	ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(*behaviorTreeAssetReferencePath);
	if (ensure(BTObject.Succeeded()))
	{
		mBehaviorTreeAsset = BTObject.Object;
	}

	ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(*blackboardAssetReferencePath);
	if (ensure(BBObject.Succeeded()))
	{
		mBlackboardAsset = BBObject.Object;
	}

	if (ensure(aiControllerClass))
	{
		AIControllerClass = aiControllerClass;
	}
}

bool AGodfallEnemyBase::IsExecutableEnemy() const
{
	return mGodfallEnemyAnimInstance->GetExecutionMontage() != nullptr;
}

AActor* AGodfallEnemyBase::GetEnemyTargetActor() const
{
	if (!mGodfallAIControllerBase) return nullptr;
	
	UBlackboardComponent* blackboard = mGodfallAIControllerBase->GetBlackboardComponent();
	if (!blackboard) return nullptr;

	return Cast<AActor>(blackboard->GetValueAsObject(TEXT("TargetActor")));
}
