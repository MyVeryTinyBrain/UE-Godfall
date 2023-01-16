// Fill out your copyright notice in the Description page of Project Settings.


#include "CCrystalCrawler.h"
#include "AnimInstanceCrystalCrawler.h"

ACCrystalCrawler::ACCrystalCrawler()
{
	InitSkeletalMesh(TEXT("/Game/Characters/CrystalCrawler/SK_Crystal_Crawler_v1.SK_Crystal_Crawler_v1"), FVector(0.0f, 0.0f, -89.819877f));
	InitAnimInstance(TEXT("/Game/Characters/CrystalCrawler/SK_Crystal_Crawler_v1_Skeleton_AnimBlueprint.SK_Crystal_Crawler_v1_Skeleton_AnimBlueprint_C"));
	InitCapsule(85.0f, 85.0f);
	InitAI(
		TEXT("/Game/Characters/CrystalCrawler/BT_CrystalCrawler.BT_CrystalCrawler"),
		TEXT("/Game/Characters/CrystalCrawler/BB_CrystalCrawler.BB_CrystalCrawler")
	);

	HeadHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("Head Hitbox"));
	HeadHitbox->InitSphereRadius(60.0f);
	HeadHitbox->SetupAttachment(GetMesh(), TEXT("head_socket"));
	InitHitboxIndex(0, HeadHitbox);

	mMaxHealth = 2.0f;
	mMaxStemina = 2.0f;
	mFloatingUIHeight = 100.0f;
	mUseStunKill = true;
}

void ACCrystalCrawler::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	USkeletalMeshComponent* characterMesh = GetMesh();
	if (ensure(characterMesh))
	{
		UAnimInstance* animInstance = characterMesh->GetAnimInstance();
		mAnimInstance = Cast<UAnimInstanceCrystalCrawler>(animInstance);
	}
}

void ACCrystalCrawler::SetupStateTable(FStateTable& mStateTable)
{
	mStateTable.AddState(TEXT("Attack1"));
}

void ACCrystalCrawler::BeginPlay()
{
	Super::BeginPlay();
}

void ACCrystalCrawler::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
}
