// Fill out your copyright notice in the Description page of Project Settings.


#include "BeamRain.h"
#include <NiagaraComponent.h>
#include "GodfallCharacterBase.h"
#include "GodfallUtil.h"

ABeamRain::ABeamRain()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	AreaEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Area Effect"));
	AreaEffect->SetupAttachment(RootComponent);
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> areaEffect(TEXT("/Game/FX/NS_DangerArea_A1.NS_DangerArea_A1"));
	if (ensure(areaEffect.Succeeded()))
	{
		AreaEffect->SetAsset(areaEffect.Object);
	}
	AreaEffect->bAutoActivate = true;

	ImpactEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Impact Effect"));
	ImpactEffect->SetupAttachment(RootComponent);
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> impactEffect(TEXT("/Game/FX/NS_BeamImpact_A1.NS_BeamImpact_A1"));
	if (ensure(impactEffect.Succeeded()))
	{
		ImpactEffect->SetAsset(impactEffect.Object);
	}
	ImpactEffect->bAutoActivate = false;

	ImpactCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Impact Collision"));
	ImpactCollision->InitSphereRadius(150.0f);
	ImpactCollision->SetupAttachment(RootComponent);
	ImpactCollision->SetCollisionProfileName(GodfallPresets::HitTrigger);
	ImpactCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BeamEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Beam Effect"));
	BeamEffect->SetupAttachment(RootComponent);
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> beamEffect(TEXT("/Game/FX/NS_Beam_1.NS_Beam_1"));
	if (ensure(beamEffect.Succeeded()))
	{
		BeamEffect->SetAsset(beamEffect.Object);
	}
	BeamEffect->bAutoActivate = false;
	UpdateBeam();


	ConstructorHelpers::FObjectFinder<USoundBase> impactSound(TEXT("/Game/Audio/Solaris/Q_Solaris_Beam.Q_Solaris_Beam"));
	if (ensure(impactSound.Succeeded()))
	{
		mImpactSound = impactSound.Object;
	}
}

void ABeamRain::BeginPlay()
{
	Super::BeginPlay();

	ImpactCollision->OnComponentBeginOverlap.AddDynamic(this, &ABeamRain::OnHitboxOverlap);
}

void ABeamRain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	mAccumulation += DeltaTime;

	switch (mState)
	{
		case EBeamRainState::Delay:
		{
			if (mAccumulation > mImpactDelay)
			{
				ImpactEffect->Activate();
				BeamEffect->Activate();
				ImpactCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

				if (ensure(mImpactSound))
				{
					UGameplayStatics::SpawnSoundAtLocation(GetWorld(), mImpactSound, GetActorLocation(), FRotator::ZeroRotator, 0.5f, 1.0f, 0.0f);
				}

				mAccumulation = 0.0f;
				mState = EBeamRainState::IncreaseBeamScale;
			}
		}
		break;
		case EBeamRainState::IncreaseBeamScale:
		{
			mEffectAlphaScale = FMath::FInterpTo(mEffectAlphaScale, 1.0f, DeltaTime, 10.0f);
			if (mEffectAlphaScale > 0.95f)
			{
				mEffectAlphaScale = 1.0f;

				mState = EBeamRainState::Working;
			}
		}
		break;
		case EBeamRainState::Working:
		{
			if (mAccumulation > mImpactDuration)
			{
				mState = EBeamRainState::DecreaseBeamScale;
			}
		}
		break;
		case EBeamRainState::DecreaseBeamScale:
		{
			mEffectAlphaScale = FMath::FInterpTo(mEffectAlphaScale, 0.0f, DeltaTime, 10.0f);
			if (mEffectAlphaScale < 0.01f)
			{
				Destroy();
			}
		}
		break;
	}

	UpdateBeam();
	UpdateImpact();
}

void ABeamRain::OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AGodfallCharacterBase* character = Cast<AGodfallCharacterBase>(OtherActor);
	if (character && IsDamagableActor(character))
	{
		FDamageInput damageInput;
		damageInput.CauserActor = this;
		damageInput.CauserCharacter = nullptr;
		damageInput.HealthDamage = 1.0f;
		damageInput.SteminaDamage = 1.0f;
		damageInput.Hitbox = ImpactCollision;
		damageInput.Power = EDamagePower::Level_3;
		damageInput.UnstoppableDamage = true;
		FDamageOutput damageOutput = character->Damage(damageInput);
	}
}

bool ABeamRain::IsDamagableActor(AActor* actor) const
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

ABeamRain* ABeamRain::SpawnBeamRain(UWorld* world, const FVector& location, bool raycast)
{
	ABeamRain* actor = world->SpawnActor<ABeamRain>(ABeamRain::StaticClass(), location, FRotator::ZeroRotator);
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

void ABeamRain::UpdateBeam()
{
	BeamEffect->SetFloatParameter(TEXT("Width"), mBeamWidth);
	BeamEffect->SetFloatParameter(TEXT("WidthScale"), mEffectAlphaScale);
	BeamEffect->SetFloatParameter(TEXT("Length"), mBeamLength);

	BeamEffect->SetRelativeLocationAndRotation(FVector::UpVector * mBeamLength, FRotator(-90.0f, 0.0f, 0.0f));
}

void ABeamRain::UpdateImpact()
{
	AreaEffect->SetFloatParameter(TEXT("Radius"), mImpactRadius);
	AreaEffect->SetFloatParameter(TEXT("Duration"), mImpactDelay);
	AreaEffect->SetFloatParameter(TEXT("FadeDuration"), mImpactDelayFadeoutTime);

	ImpactEffect->SetFloatParameter(TEXT("Radius"), mImpactRadius);
	ImpactEffect->SetFloatParameter(TEXT("Alpha"), mEffectAlphaScale);

	ImpactCollision->SetSphereRadius(mImpactRadius);
}

