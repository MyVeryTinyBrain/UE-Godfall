// Fill out your copyright notice in the Description page of Project Settings.


#include "PlasmaZone.h"
#include <NiagaraComponent.h>
#include "GodfallCharacterBase.h"
#include "GodfallUtil.h"

APlasmaZone::APlasmaZone()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	PlasmaZoneEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Beam Effect"));
	PlasmaZoneEffect->SetupAttachment(RootComponent);
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> plasmaZoneEffect(TEXT("/Game/FX/NS_PlasmaZone.NS_PlasmaZone"));
	if (ensure(plasmaZoneEffect.Succeeded()))
	{
		PlasmaZoneEffect->SetAsset(plasmaZoneEffect.Object);
	}

	PlasmaZoneCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Plasma Zone Collision"));
	PlasmaZoneCollision->InitSphereRadius(mPlasmaZoneRadius);
	PlasmaZoneCollision->SetupAttachment(RootComponent);
	PlasmaZoneCollision->SetCollisionProfileName(GodfallPresets::HitTrigger);
	PlasmaZoneCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UpdatePlasmaZone();
}

void APlasmaZone::BeginPlay()
{
	Super::BeginPlay();
}

void APlasmaZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (mReady)
	{
		mReady = false;

		PlasmaZoneCollision->OnComponentBeginOverlap.AddDynamic(this, &APlasmaZone::OnHitboxOverlap);
		PlasmaZoneCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	mAccumulation += DeltaTime;

	switch (mState)
	{
		case EPlasmaZoneState::Working:
		{
			if (mAccumulation > mPlasmaZoneDuration)
			{
				mAccumulation = 0.0f;
				mState = EPlasmaZoneState::Destorying;
			}
		}
		break;
		case EPlasmaZoneState::Destorying:
		{
			mAlphaScale = 1.0f - (mAccumulation / mPlasmaZoneFadeOutTime);
			if (mAccumulation > mPlasmaZoneFadeOutTime)
			{
				Destroy();
			}
		}
		break;
	}

	UpdatePlasmaZone();
}

void APlasmaZone::OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (mAlphaScale < 0.3f) return;

	AGodfallCharacterBase* character = Cast<AGodfallCharacterBase>(OtherActor);
	if (character && IsDamagableActor(character))
	{
		FDamageInput damageInput;
		damageInput.CauserActor = this;
		damageInput.CauserCharacter = nullptr;
		damageInput.HealthDamage = 1.0f;
		damageInput.SteminaDamage = 1.0f;
		damageInput.Hitbox = PlasmaZoneCollision;
		damageInput.Power = EDamagePower::Level_3;
		damageInput.UnstoppableDamage = true;
		FDamageOutput damageOutput = character->Damage(damageInput);
	}
}

bool APlasmaZone::IsDamagableActor(AActor* actor) const
{
	if (!actor) return false;

	if (mUseCollisionTags && !mCollisionTags.IsEmpty())
	{
		if (!GodfallUtil::String::CompareNames(actor->Tags, mCollisionTags))
		{
			return false;
		}
	}

	for (auto& ignoreActor : mIgnoreActors)
	{
		if (ignoreActor == actor)
		{
			return false;
		}
	}

	return true;
}

void APlasmaZone::UpdatePlasmaZone()
{
	PlasmaZoneEffect->SetFloatParameter(TEXT("Radius"), mPlasmaZoneRadius);
	PlasmaZoneEffect->SetColorParameter(TEXT("Color"), mColor);
	PlasmaZoneEffect->SetFloatParameter(TEXT("Alpha"), mAlpha * mAlphaScale);

	PlasmaZoneCollision->SetSphereRadius(mPlasmaZoneRadius);
}

APlasmaZone* APlasmaZone::SpawnPlasmaZone(UWorld* world, const FVector& location, bool raycast)
{
	APlasmaZone* actor = world->SpawnActor<APlasmaZone>(APlasmaZone::StaticClass(), location, FRotator::ZeroRotator);
	if (!actor) return nullptr;

	if (raycast)
	{
		FCollisionQueryParams params(NAME_None, true, actor);
		FHitResult hit;
		bool result = world->LineTraceSingleByProfile(
			hit,
			location, location + FVector::DownVector * 10000.0f,
			GodfallPresets::SweepCollision, params);

		if (result)
		{
			actor->SetActorLocation(hit.ImpactPoint);
		}
	}

	return actor;
}

