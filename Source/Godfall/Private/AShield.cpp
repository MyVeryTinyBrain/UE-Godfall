// Fill out your copyright notice in the Description page of Project Settings.


#include "AShield.h"
#include "AnimInstanceShield.h"

// Sets default values
AAShield::AAShield()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ShieldMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Shield Mesh"));
	RootComponent = ShieldMesh;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_shiledBase(TEXT("/Game/Weapons/ShieldBase/SK_shieldBase.SK_shieldBase"));
	if (ensure(SK_shiledBase.Succeeded()))
	{
		ShieldMesh->SetSkeletalMesh(SK_shiledBase.Object);
	}

	ConstructorHelpers::FClassFinder<UAnimInstance> animInstanceClass(TEXT("/Game/Weapons/ShieldBase/SK_shieldBase_Skeleton_AnimBlueprint.SK_shieldBase_Skeleton_AnimBlueprint_C"));
	if (ensure(animInstanceClass.Succeeded()))
	{
		ShieldMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		ShieldMesh->SetAnimInstanceClass(animInstanceClass.Class);
	}

	SetShieldVisibility(false);
}

bool AAShield::IsOpen() const
{
	UAnimInstanceShield* animInstance = Cast<UAnimInstanceShield>(ShieldMesh->GetAnimInstance());
	if (ensure(animInstance))
	{
		if (animInstance->mOpen)
		{
			return true;
		}

		return animInstance->IsOpenToAny();
	}

	return false;
}

void AAShield::SetOpen(bool value)
{
	if (!ensure(ShieldMesh)) return;

	UAnimInstanceShield* animInstance = Cast<UAnimInstanceShield>(ShieldMesh->GetAnimInstance());
	if (ensure(animInstance))
	{
		animInstance->mOpen = value;
	}
}

// Called when the game starts or when spawned
void AAShield::BeginPlay()
{
	Super::BeginPlay();

	UAnimInstanceShield* animInstance = Cast<UAnimInstanceShield>(ShieldMesh->GetAnimInstance());
	if (ensure(animInstance))
	{
		animInstance->OnShieldClosed.AddDynamic(this, &AAShield::OnShieldClosed);
		animInstance->OnShieldOpen.AddDynamic(this, &AAShield::OnShieldOpen);
	}
}

// Called every frame
void AAShield::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAShield::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AAShield::OnShieldClosed()
{
	SetShieldVisibility(false);
}

void AAShield::OnShieldOpen()
{
	SetShieldVisibility(true);
}

void AAShield::SetShieldVisibility(bool value)
{
	ShieldMesh->SetVisibility(value);
}

