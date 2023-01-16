// Fill out your copyright notice in the Description page of Project Settings.


#include "CSolaris.h"
#include "AnimInstanceSolaris.h"
#include "Beam.h"
#include "BeamRain.h"
#include "PlasmaZone.h"
#include "GodfallGameState.h"
#include "EventManagerComponent.h"

ACSolaris::ACSolaris()
{
	InitSkeletalMesh(TEXT("/Game/Characters/Solaris/SK_Solaris.SK_Solaris"), FVector(0.0f, 0.0f, -250.0f));
	InitAnimInstance(TEXT("/Game/Characters/Solaris/SK_Solaris_Skeleton_AnimBlueprint.SK_Solaris_Skeleton_AnimBlueprint_C"));
	InitCapsule(250.0f, 170.0f);
	InitAI(
		TEXT("/Game/Characters/Solaris/BT_Solaris.BT_Solaris"),
		TEXT("/Game/Characters/Solaris/BB_Solaris.BB_Solaris")
	);

	USkeletalMeshComponent* characterMesh = GetMesh();
	if (ensure(characterMesh))
	{
		characterMesh->SetTeleportDistanceThreshold(100.0f);
		characterMesh->SetTeleportRotationThreshold(45.0f);
		characterMesh->bLocalSpaceSimulation = true;
		characterMesh->ClothBlendWeight = 1.0f;
	}

	// GetMesh()->bCollideWithEnvironment = true;
	// 버그 있습니다.
	// 복잡한 콜라이더와 충돌 시 예외 발생합니다.

	WeaponRHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Right Weapon Hitbox"));
	WeaponRHitbox->SetRelativeLocationAndRotation(FVector(0.0f, -160.0f, -10.0f), FRotator(0.0f, 0.0f, 90.0f));
	WeaponRHitbox->InitCapsuleSize(80.0f, 260.0f);
	WeaponRHitbox->SetupAttachment(GetMesh(), TEXT("switch_blade_socket"));
	InitHitboxIndex(0, WeaponRHitbox);

	ArmRHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Right Arm Hitbox"));
	ArmRHitbox->SetRelativeLocationAndRotation(FVector(-110.0f, 0.0f, -10.0f), FRotator(0.0f, -90.0f, 90.0f));
	ArmRHitbox->InitCapsuleSize(80.0f, 260.0f);
	ArmRHitbox->SetupAttachment(GetMesh(), TEXT("lowerarm_r_socket"));
	InitHitboxIndex(0, ArmRHitbox);

	BigHandLHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Big Left Hand Hitbox"));
	BigHandLHitbox->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
	BigHandLHitbox->InitCapsuleSize(600.0f, 600.0f);
	BigHandLHitbox->SetupAttachment(GetMesh(), TEXT("hand_l_socket"));
	InitHitboxIndex(1, BigHandLHitbox);

	BigWeaponRHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Big Right Weapon Hitbox"));
	BigWeaponRHitbox->SetRelativeLocationAndRotation(FVector(0.0f, -160.0f, -10.0f), FRotator(0.0f, 0.0f, 90.0f));
	BigWeaponRHitbox->InitCapsuleSize(300.0f, 300.0f);
	BigWeaponRHitbox->SetupAttachment(GetMesh(), TEXT("switch_blade_socket"));
	InitHitboxIndex(2, BigWeaponRHitbox);

	mMaxHealth = 160.0f;
	mMaxStemina = 65.0f;
	mExecutionDamage = 40.0f;
	mBlockSteminaDamage = 2.0f;
	mDamagableType = ECharacterDamagableType::AdditiveOnly;
	mFloatingUIHeight = 200.0f;
}

void ACSolaris::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	USkeletalMeshComponent* characterMesh = GetMesh();
	if (ensure(characterMesh))
	{
		UAnimInstance* animInstance = characterMesh->GetAnimInstance();
		mAnimInstance = Cast<UAnimInstanceSolaris>(animInstance);
	}
}

void ACSolaris::SetupStateTable(FStateTable& mStateTable)
{
	mStateTable.AddState(TEXT("Attack1"));
	mStateTable.AddState(TEXT("Attack2"));
	mStateTable.AddState(TEXT("Attack3"));
	mStateTable.AddState(TEXT("Attack4"));
	mStateTable.AddState(TEXT("Attack5"));
	mStateTable.AddState(TEXT("Attack6"));
	mStateTable.AddState(TEXT("Attack7"));
	mStateTable.AddState(TEXT("Attack8"));
	mStateTable.AddState(TEXT("Attack9"));
	mStateTable.AddState(TEXT("Attack10"));
	mStateTable.AddState(TEXT("Attack11"));
	mStateTable.AddState(TEXT("Attack12"));
	mStateTable.AddState(TEXT("Attack13"));
	mStateTable.AddState(TEXT("Attack14"));
}

void ACSolaris::BeginPlay()
{
	Super::BeginPlay();

	mAnimInstance->OnCustomAnimNotifyState.AddDynamic(this, &ACSolaris::OnCustomAnimNotifyState);

	Beam = GetWorld()->SpawnActor<ABeam>(ABeam::StaticClass());
	if (ensure(Beam))
	{
		FAttachmentTransformRules attachmentRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
		Beam->AttachToComponent(GetMesh(), attachmentRules, TEXT("beam_socket"));

		Beam->mIgnoreActors.Add(this);
		//Beam->mUseCollisionTags = true;
		Beam->SetActiveDirect(false);
	}
}

void ACSolaris::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}

void ACSolaris::OnDead()
{
	Super::OnDead();

	AGodfallGameState* gameState = GetWorld()->GetGameState<AGodfallGameState>();
	if (ensure(gameState))
	{
		gameState->GetEventManager()->NotifyEvent(this, TEXT("Dead"), TEXT(""));
	}
}

void ACSolaris::OnCustomAnimNotifyState(EAnimNotifyStateEvent eEvent, const FAnimNotifiyStateData& data, const FName& notifyStateName, const FCustomNotifyStateVariable& variables)
{
	if (notifyStateName == TEXT("Beam"))
	{
		switch (eEvent)
		{
			case EAnimNotifyStateEvent::Begin:
			{
				Beam->SetActive(true);
			}
			break;
			case EAnimNotifyStateEvent::End:
			{
				Beam->SetActive(false);
			}
			break;
		}
	}
	else if (notifyStateName == TEXT("BeamRains"))
	{
		switch (eEvent)
		{
			case EAnimNotifyStateEvent::Begin:
			{
				mBeamRainAccumulation = 0.0f;
				SpawnBeamRainToTarget(200.0f, 0.6f, 1.5f, 50.0f);
			}
			break;
			case EAnimNotifyStateEvent::Tick:
			{
				mBeamRainAccumulation += data.FrameDeltaTime;

				if (mBeamRainAccumulation > mBeamRainInterval)
				{
					mBeamRainAccumulation = 0.0f;
					SpawnBeamRainToTarget(200.0f, 0.6f, 1.5f, 50.0f);
				}
			}
			break;
		}
	}
	else if (notifyStateName == TEXT("BigBeamRain"))
	{
		switch (eEvent)
		{
			case EAnimNotifyStateEvent::Begin:
			{
				mBeamRainAccumulation = 0.0f;
				SpawnBeamRainToTarget(1000.0f, 2.0f, 1.0f, 800.0f);
			}
			break;
		}
	}
	else if (notifyStateName == TEXT("ShieldBurst"))
	{
		switch (eEvent)
		{
			case EAnimNotifyStateEvent::Begin:
			{
				if (!ensure(variables.Floatings.Num() > 0)) break;

				SpawnPlasmaZoneOnGround(variables.Floatings[0], 1.0f);
			}
			break;
		}
	}
}

void ACSolaris::SpawnBeamRainToTarget(float radius, float delay, float duration, float width)
{
	AActor* target = GetEnemyTargetActor();
	if (!target) return;

	ABeamRain* beamRain = ABeamRain::SpawnBeamRain(GetWorld(), target->GetActorLocation(), true);
	if (!beamRain) return;
	
	beamRain->mIgnoreActors.Add(this);
	beamRain->mImpactRadius = radius;
	beamRain->mImpactDelay = delay;
	beamRain->mImpactDuration = duration;
	beamRain->mBeamWidth = width;
}

void ACSolaris::SpawnPlasmaZoneOnGround(float radius, float duration)
{
	APlasmaZone* plasmaZone = APlasmaZone::SpawnPlasmaZone(GetWorld(), GetActorLocation(), true);
	if (!plasmaZone) return;

	plasmaZone->mIgnoreActors.Add(this);
	plasmaZone->mPlasmaZoneRadius = radius;
	plasmaZone->mPlasmaZoneDuration = duration;
}
