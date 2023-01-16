// Fill out your copyright notice in the Description page of Project Settings.


#include "Beam.h"
#include <NiagaraComponent.h>
#include "HitboxComponent.h"
#include "GodfallCharacterBase.h"
#include "GodfallUtil.h"
#include <Components/DecalComponent.h>

ABeam::ABeam()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BeamEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Beam Effect"));
	BeamEffect->SetupAttachment(RootComponent);
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> beamEffect(TEXT("/Game/FX/NS_Beam_1.NS_Beam_1"));
	if (ensure(beamEffect.Succeeded()))
	{
		BeamEffect->SetAsset(beamEffect.Object);
	}

	BeamCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Beam Collision"));
	BeamCollision->SetRelativeRotation(FRotator(0.0f, 90.0f, 90.0f));
	BeamCollision->InitCapsuleSize(1.0f, 1.0f);
	BeamCollision->SetupAttachment(RootComponent);
	BeamCollision->SetCollisionProfileName(GodfallPresets::HitTrigger);
	BeamCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConstructorHelpers::FObjectFinder<UMaterial> decalMaterial(TEXT("/Game/FX/Materials/M_BeamDecal_A1.M_BeamDecal_A1"));
	if (ensure(decalMaterial.Succeeded()))
	{
		BeamDecalMaterial = decalMaterial.Object;
	}
}

void ABeam::BeginPlay()
{
	Super::BeginPlay();
}

void ABeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (mReady)
	{
		mReady = false;

		BeamCollision->OnComponentBeginOverlap.AddDynamic(this, &ABeam::OnHitboxOverlap);
		BeamCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	UpdateBeam();
	UpdateScale(DeltaTime);
}

void ABeam::OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!mBeamActive) return;
	if (mBeamScale < 0.1f) return;

	AGodfallCharacterBase* character = Cast<AGodfallCharacterBase>(OtherActor);
	if (character && IsDamagableActor(character))
	{
		FDamageInput damageInput;
		damageInput.CauserActor = this;
		damageInput.CauserCharacter = nullptr;
		damageInput.HealthDamage = 1.0f;
		damageInput.SteminaDamage = 1.0f;
		damageInput.Hitbox = BeamCollision;
		damageInput.Power = EDamagePower::Level_3;
		damageInput.UnstoppableDamage = true;
		FDamageOutput damageOutput = character->Damage(damageInput);
	}
}

FVector ABeam::GetBeamMaxEndPoint() const
{
	return GetActorLocation() + GetActorForwardVector() * mBeamMaxLength;
}

bool ABeam::IsDamagableActor(AActor* actor) const
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

void ABeam::SetActiveDirect(bool value)
{
	mBeamActive = value;
	mBeamScale = value ? 1.0f : 0.0f;
	SetActorEnableCollision(value);
	UpdateBeam();
}

void ABeam::SetActive(bool value)
{
	if (mBeamActive == value)
	{
		return;
	}

	mBeamActive = value;

	//SetActorHiddenInGame(!value);
	SetActorEnableCollision(value);
	//SetActorTickEnabled(value);

	UpdateBeam();
}

bool ABeam::GetActive() const
{
	return mBeamActive;
}

void ABeam::UpdateBeam()
{
	BeamEffect->SetFloatParameter(TEXT("Width"), mBeamWidth);
	BeamEffect->SetFloatParameter(TEXT("WidthScale"), mBeamWidthScale * mBeamScale);

	bool result = false; 
	FHitResult hit;
	if (mBeamScale > 0.0f)
	{
		FCollisionQueryParams params(NAME_None, true);
		result = GetWorld()->LineTraceSingleByProfile(
			hit,
			GetActorLocation(), GetBeamMaxEndPoint(),
			GodfallPresets::SweepCollision,
			params);
	}

	float beamLength = mBeamMaxLength;
	if (result)
	{
		beamLength = FVector::Distance(hit.ImpactPoint, GetActorLocation());
		
		UDecalComponent* decal = UGameplayStatics::SpawnDecalAtLocation(
			GetWorld(),
			BeamDecalMaterial,
			mBeamWidth * mBeamWidthScale * mBeamScale * FVector::OneVector,
			hit.ImpactPoint,
			FRotator::ZeroRotator);
		if (decal)
		{
			decal->SetFadeOut(0.5f, 1.0f, true);
			decal->SetFadeScreenSize(0.0f);
		}
	}
	BeamEffect->SetFloatParameter(TEXT("Length"), beamLength);
	BeamCollision->SetCapsuleRadius(mBeamWidth * 0.5f * mBeamWidthScale);
	BeamCollision->SetCapsuleHalfHeight(beamLength * 0.5f);
	BeamCollision->SetRelativeLocation(FVector::ForwardVector * beamLength * 0.5f);
}

void ABeam::UpdateScale(float deltaTime)
{
	float targetBeamScale = mBeamActive ? 1.0f : 0.0f;
	mBeamScale = FMath::FInterpTo(mBeamScale, targetBeamScale, deltaTime, 10.0f);
	if (FMath::Abs(mBeamScale - targetBeamScale) < 0.01f)
	{
		mBeamScale = targetBeamScale;
	}
}

