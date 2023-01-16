// Fill out your copyright notice in the Description page of Project Settings.


#include "CGrievesMinion.h"
#include "AnimInstanceEarthAberrant.h"

ACGrievesMinion::ACGrievesMinion()
{
	InitSkeletalMesh(TEXT("/Game/Characters/GrievesMinion_Armored/SK_Earth_Aberrant_A.SK_Earth_Aberrant_A"), FVector(0.0f, 0.0f, -97.475722f));
	InitAnimInstance(TEXT("/Game/Characters/GrievesMinion_Armored/SK_Earth_Aberrant_A_Skeleton_AnimBlueprint.SK_Earth_Aberrant_A_Skeleton_AnimBlueprint_C"));
	InitCapsule(100.0f, 70.0f);
	InitAI(
		TEXT("/Game/Characters/GrievesMinion_Armored/BT_EarthAberrant.BT_EarthAberrant"),
		TEXT("/Game/Characters/GrievesMinion_Armored/BB_EarthAberrant.BB_EarthAberrant")
	);

	WeaponRSocketComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon RSocket"));
	WeaponRSocketComponent->SetupAttachment(GetMesh(), TEXT("weapon_r_socket"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	WeaponMesh->SetupAttachment(WeaponRSocketComponent);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> weapon(TEXT("/Game/Weapons/Enemy/EarthPick/SK_EarthPick.SK_EarthPick"));
	if (ensure(weapon.Succeeded())) WeaponMesh->SetSkeletalMesh(weapon.Object);

	WeaponHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Weapon Hitbox"));
	WeaponHitbox->SetRelativeLocationAndRotation(FVector(0.0f, 5.0f, 70.0f), FRotator(0.0f, 0.0f, -90.0f));
	WeaponHitbox->InitCapsuleSize(22.0f, 40.0f);
	WeaponHitbox->SetupAttachment(WeaponMesh);
	InitHitboxIndex(0, WeaponHitbox);

	mMaxHealth = 8.0f;
	mMaxStemina = 8.0f;
	mFloatingUIHeight = 50.0f;
}

void ACGrievesMinion::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	USkeletalMeshComponent* characterMesh = GetMesh();
	if (ensure(characterMesh))
	{
		UAnimInstance* animInstance = characterMesh->GetAnimInstance();
		mAnimInstance = Cast<UAnimInstanceEarthAberrant>(animInstance);
	}
}

void ACGrievesMinion::SetupStateTable(FStateTable& mStateTable)
{
	mStateTable.AddState(TEXT("Attack1"));
	mStateTable.AddState(TEXT("Attack2"));
	mStateTable.AddState(TEXT("Attack3"));
	mStateTable.AddState(TEXT("Attack4"));
}

void ACGrievesMinion::BeginPlay()
{
	Super::BeginPlay();
}

void ACGrievesMinion::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}
