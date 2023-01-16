// Fill out your copyright notice in the Description page of Project Settings.


#include "GodfallCharacterBase.h"
#include "GodfallAnimInstanceBase.h"
#include "GodfallCharacterAnimInstanceBase.h"
#include "CharacterRotateComponent.h"
#include "HitboxComponent.h"
#include "GodfallGameState.h"
#include "CharacterManagerComponent.h"

// Sets default values
AGodfallCharacterBase::AGodfallCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->MaxStepHeight = 50.0f;
	GetCharacterMovement()->SetWalkableFloorAngle(50.0f);

	CharacterRotate = CreateDefaultSubobject<UCharacterRotateComponent>(TEXT("Character Rotate"));
	Hitbox = CreateDefaultSubobject<UHitboxComponent>(TEXT("Hitbox"));

	Tags.Add(GodfallTags::Character);

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> hitEffect(TEXT("/Game/FX/NS_Hit.NS_Hit"));
	if (ensure(hitEffect.Succeeded()))
	{
		mHitEffect = hitEffect.Object;
	}

	ConstructorHelpers::FObjectFinder<USoundBase> damageSound(TEXT("/Game/Audio/Common/Q_Hit.Q_Hit"));
	if (ensure(damageSound.Succeeded()))
	{
		mDamageSound = damageSound.Object;
	}

	ConstructorHelpers::FObjectFinder<USoundBase> footstepSound(TEXT("/Game/Audio/Footstep/Q_Footstep_1.Q_Footstep_1"));
	if (ensure(footstepSound.Succeeded()))
	{
		mFootstepSound = footstepSound.Object;
	}
}

void AGodfallCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	USkeletalMeshComponent* characterMesh = GetMesh();
	if (ensure(characterMesh))
	{
		UAnimInstance* animInstance = characterMesh->GetAnimInstance();
		mGodfallAnimInstance = Cast<UGodfallAnimInstanceBase>(animInstance);
		mGodfallCharacterAnimInstance = Cast<UGodfallCharacterAnimInstanceBase>(animInstance);
	}
}

// Called when the game starts or when spawned
void AGodfallCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (Hitbox)
	{
		Hitbox->OnOverlap.AddDynamic(this, &AGodfallCharacterBase::OnHitboxOverlap);
	}

	OnDestroyed.AddDynamic(this, &AGodfallCharacterBase::OnDestroyedCallback);

	AGodfallGameState* gameState = Cast<AGodfallGameState>(GetWorld()->GetGameState());
	if (ensure(gameState))
	{
		gameState->GetCharacterManager()->RegistCharacter(this);
	}

	if (mGodfallCharacterAnimInstance)
	{
		mGodfallCharacterAnimInstance->OnDamagableEvent.AddDynamic(this, &AGodfallCharacterBase::OnDamagableEvent);
		mGodfallCharacterAnimInstance->OnRotateEvent.AddDynamic(this, &AGodfallCharacterBase::OnRotateEvent);
		mGodfallCharacterAnimInstance->OnBeginStun.AddUObject(this, &AGodfallCharacterBase::OnBeginStun);
		mGodfallCharacterAnimInstance->OnEndStun.AddUObject(this, &AGodfallCharacterBase::OnEndStun);
		mGodfallCharacterAnimInstance->OnEndDeath.AddUObject(this, &AGodfallCharacterBase::OnEndDeath);
		mGodfallCharacterAnimInstance->OnAnimNotifyFootstep.AddDynamic(this, &AGodfallCharacterBase::OnAnimNotifyFootstep);
	}

	if (mSetHealthFromMaxHealth)
	{
		mHealth = mMaxHealth;
	}
}

// Called every frame
void AGodfallCharacterBase::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	CharacterRotate->mRotate = CanRotate();
}

// Called to bind functionality to input
void AGodfallCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AGodfallCharacterBase::OnDestroyedCallback(AActor* destroyed)
{
	AGodfallGameState* gameState = Cast<AGodfallGameState>(GetWorld()->GetGameState());
	if (ensure(gameState))
	{
		gameState->GetCharacterManager()->UnregistCharacter(this);
	}
}

void AGodfallCharacterBase::OnDamagableEvent(EAnimNotifyStateEvent eEvent, const FDamagableData& data)
{
	switch (eEvent)
	{
		case EAnimNotifyStateEvent::Begin:
		{
			mDamagableData = data;
			Hitbox->SetHitboxCollision(data.HitboxIndex, true);
		}
		break;
		case EAnimNotifyStateEvent::End:
		{
			Hitbox->SetHitboxCollision(data.HitboxIndex, false);
			Hitbox->ResetHitBuffer(data.HitboxIndex);
		}
		break;
	}
}

void AGodfallCharacterBase::OnRotateEvent(EAnimNotifyStateEvent eEvent, const FRotateOverride& data)
{
	switch (eEvent)
	{
		case EAnimNotifyStateEvent::Begin:
		case EAnimNotifyStateEvent::End:
		{
			CharacterRotate->mOverride = data;
		}
		break;
	}
}

void AGodfallCharacterBase::OnAnimNotifyFootstep()
{
	PlayFootstepSound();
}

void AGodfallCharacterBase::OnHitboxOverlap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
	AGodfallCharacterBase* character = Cast<AGodfallCharacterBase>(OtherActor);
	if (character)
	{
		int hitboxIndex = Hitbox->GetHitboxIndex(OverlappedComponent);
		if (hitboxIndex < 0) return;

		if (Hitbox->AddActorToHitBuffer(hitboxIndex, OtherActor))
		{
			FDamagableData damagableData = mDamagableData;

			FVector otherForward = character->GetActorForwardVector();
			FVector otherToThis = GetActorLocation() - character->GetActorLocation();
			otherToThis = FVector::VectorPlaneProject(otherToThis, FVector::UpVector);
			otherToThis.Normalize();
			float d = FVector::DotProduct(otherForward, otherToThis);
			if (d > 0.0f) damagableData.DamageDirection = (damagableData.DamageDirection == EDamageDirection::Left ? EDamageDirection::Right : EDamageDirection::Left);

			FDamagableInput damagableInput;
			damagableInput.Data = damagableData;
			damagableInput.Hitbox = OverlappedComponent;

			FDamageInput damageInput = OnHit(damagableInput);

			FDamageOutput damageOutput = character->Damage(damageInput);
			OnPostHit(damageOutput);
		}
	}
}

FDamageInput AGodfallCharacterBase::OnHit(const FDamagableInput& damagableData)
{
	FDamageInput damageInput;
	damageInput.CauserActor = this;
	damageInput.CauserCharacter = this;
	damageInput.HealthDamage = damagableData.Data.HealthDamageMultiplier * mHealthDamage;
	damageInput.SteminaDamage = damagableData.Data.SteminaDamageMultiplier * mSteminaDamage;
	damageInput.UnstoppableDamage = damagableData.Data.UnstoppableDamage;
	damageInput.Power = damagableData.Data.Power;
	damageInput.DamageDirection = damagableData.Data.DamageDirection;
	damageInput.Hitbox = damagableData.Hitbox;

	return damageInput;
}

FDamagePreOutput AGodfallCharacterBase::CalcDamage(const FDamageInput& input) const
{
	EBlockOutput block = CanBlock(input);

	if (input.UnstoppableDamage)
	{
		block = EBlockOutput::Fail;
	}

	FDamagePreOutput output;
	output.CauserActor = input.CauserActor;
	output.CauserCharacter = input.CauserCharacter;
	output.Power = input.Power;
	output.DamageDirection = input.DamageDirection;
	output.Hitbox = input.Hitbox;

	switch (block)
	{
		case EBlockOutput::Fail:
		{
			output.HealthDamage = input.HealthDamage;
			output.SteminaDamage = input.SteminaDamage;
			output.Result = EDamageOutput::Hit;
		}
		break;
		case EBlockOutput::Blocked:
		{
			output.HealthDamage = 0.0f;
			output.SteminaDamage = input.SteminaDamage;
			output.Result = EDamageOutput::Blocked;
		}
		break;
		case EBlockOutput::Blocked_Perfect:
		{
			output.HealthDamage = 0.0f;
			output.SteminaDamage = input.SteminaDamage;
			output.Result = EDamageOutput::Blocked_Perfect;
		}
		break;
	}

	return output;
}

void AGodfallCharacterBase::OnDamage(const FDamageOutput& output)
{
	float stunKillDamage = (mUseStunKill && IsStun()) ? mHealth : 0.0f;
	bool onZeroHealth, onMaxStemina;
	SetHealth(mHealth - output.DamageOutput.HealthDamage - stunKillDamage, onZeroHealth);
	SetStemina(mStemina + output.DamageOutput.SteminaDamage, onMaxStemina);

	if (onMaxStemina && output.DamageOutput.Result == EDamageOutput::Blocked_Perfect)
	{
		SetStemina(mMaxStemina - 0.01f);
		onMaxStemina = false;
	}

	if (onZeroHealth) OnZeroHealth();
	if (onMaxStemina) OnMaxStemina();

	if (!onZeroHealth && !onMaxStemina)
	{
		switch (output.DamageOutput.Result)
		{
			case EDamageOutput::Hit:
			{
				switch (mDamagableType)
				{
					case ECharacterDamagableType::AllDamage:
						PlayDamageMontage(output);
						break;
					case ECharacterDamagableType::AdditiveOnly:
						PlayAdditiveDamageMontage(output);
						break;
					case ECharacterDamagableType::NoDamage:
						break;
				}
			}
			break;
			case EDamageOutput::Blocked:
			case EDamageOutput::Blocked_Perfect:
				OnGuard(output);
				break;
		}
	}
	else if (onZeroHealth)
	{
		mGodfallCharacterAnimInstance->PlayDeathMontage();
	}
	else if (onMaxStemina)
	{
		mGodfallCharacterAnimInstance->PlayStunMontage();
	}

	bool hitRotate = false;

	switch (mDamagableType)
	{
		case ECharacterDamagableType::AllDamage:
		{
			switch (output.DamageOutput.Result)
			{
				case EDamageOutput::Hit:
				{
					hitRotate = (output.DamageOutput.Power == EDamagePower::Level_3);
				}
				break;
			}
		}
		break;
	}

	if (hitRotate)
	{
		FVector causerToThis = GetActorLocation() - output.DamageOutput.CauserActor->GetActorLocation();
		causerToThis = FVector::VectorPlaneProject(causerToThis, FVector::UpVector);
		causerToThis.Normalize();

		CharacterRotate->SetRotatorDirect(FRotationMatrix::MakeFromX(-causerToThis).Rotator());
	}

	switch (output.DamageOutput.Result)
	{
		case EDamageOutput::Hit:
		{
			PlayDamageSound(output);
		}
		break;
	}

	switch (output.DamageOutput.Result)
	{
		case EDamageOutput::Hit:
		case EDamageOutput::Blocked:
		case EDamageOutput::Blocked_Perfect:
		{
			/* 타격 이펙트를 생성합니다. */

			UPrimitiveComponent* hitbox = output.DamageOutput.Hitbox;

			FCollisionShape hitboxShape = hitbox->GetCollisionShape();
			hitboxShape.Capsule.Radius *= 3.0f;
			hitboxShape.Capsule.HalfHeight *= 3.0f;

			FCollisionQueryParams params;
			params.AddIgnoredActor(this);

			/* 캐릭터 메쉬와 스윕 쿼리합니다. */

			TArray<FHitResult> results;
			GetWorld()->SweepMultiByProfile(
				results,
				hitbox->GetComponentLocation() + FVector(-1.0f),
				hitbox->GetComponentLocation() + FVector(+1.0f),
				hitbox->GetComponentRotation().Quaternion(),
				GodfallPresets::SweepPawn,
				hitboxShape,
				params
			);

			/* 쿼리 실패시 사용하는 기본값을 설정합니다. */

			// 캐릭터와 히트박스의 중간 위치
			FVector hitLocation = (output.DamagedCharacter->GetActorLocation() + hitbox->GetComponentLocation()) * 0.5f;

			// 캐릭터에서 히트박스로의 방향
			FVector hitNormal = (hitbox->GetComponentLocation() - output.DamagedCharacter->GetActorLocation()).GetSafeNormal();

			for (auto& result : results)
			{
				if (result.GetActor() == output.DamagedCharacter &&
					result.GetComponent()->GetCollisionProfileName() == GodfallPresets::CharacterMesh)
				{
					hitLocation = result.ImpactPoint;
					//hitNormal = result.ImpactNormal;
					break;
				}
			}

			FFXSystemSpawnParameters SpawnParams;
			SpawnParams.WorldContextObject = GetWorld();
			SpawnParams.SystemTemplate = mHitEffect;
			SpawnParams.Location = hitLocation;
			SpawnParams.Rotation = FRotationMatrix::MakeFromX(hitNormal).Rotator();
			UNiagaraFunctionLibrary::SpawnSystemAtLocationWithParams(SpawnParams);
		}
		break;
	}

	OnDamageImpl(output, onZeroHealth, onMaxStemina);
}

void AGodfallCharacterBase::PlayDamageMontage(const FDamageOutput& damageOutput)
{
	switch (damageOutput.DamageOutput.Power)
	{
		case EDamagePower::Level_1:
			mGodfallCharacterAnimInstance->PlayDamageMontage(EDamage::Flinch, damageOutput.DamageOutput.DamageDirection);
			break;
		case EDamagePower::Level_2:
			mGodfallCharacterAnimInstance->PlayDamageMontage(EDamage::Stagger, damageOutput.DamageOutput.DamageDirection);
			break;
		case EDamagePower::Level_3:
			mGodfallCharacterAnimInstance->PlayDamageMontage(EDamage::Knockback, damageOutput.DamageOutput.DamageDirection);
			break;
	}
}

void AGodfallCharacterBase::PlayAdditiveDamageMontage(const FDamageOutput& damageOutput)
{
	mGodfallCharacterAnimInstance->PlayAdditiveDamageMontage(damageOutput.DamageOutput.DamageDirection);
}

void AGodfallCharacterBase::OnPostHit(const FDamageOutput& damageOutput)
{
	switch (damageOutput.DamageOutput.Result)
	{
		case EDamageOutput::Blocked_Perfect:
		{
			/* 공격이 완벽히 방어당해 스태미나가 최대치가 되었을 때 스턴을 재생합니다. */

			bool onMaxStemina;
			SetStemina(mStemina + mBlockSteminaDamage, onMaxStemina);
			if (onMaxStemina)
			{
				OnMaxStemina();
				mGodfallCharacterAnimInstance->PlayStunMontage();
			}
		}
		break;
	}
}

void AGodfallCharacterBase::PlayDamageSound(const FDamageOutput& output)
{
	if (!ensure(mDamageSound)) return;
	
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), mDamageSound, GetActorLocation(), FRotator::ZeroRotator, 1.0f, 1.0f, 0.0f);
}

void AGodfallCharacterBase::PlayFootstepSound()
{
	if (!ensure(mFootstepSound)) return;

	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), mFootstepSound, GetActorLocation(), FRotator::ZeroRotator, 1.0f, 1.0f, 0.0f);
}

void AGodfallCharacterBase::OnEndStun(bool interrupted)
{
	if (mHealth > 0.0f)
	{
		SetStemina(0.0f);
	}
}

void AGodfallCharacterBase::OnEndDeath()
{
	ToDead();
}

void AGodfallCharacterBase::ToDead()
{
	if (mDead) return;
	mDead = true;
	OnDead();
}

void AGodfallCharacterBase::OnDead()
{
	Destroy();
}

FDamageOutput AGodfallCharacterBase::Damage(const FDamageInput& input)
{
	FDamagePreOutput damagePreOutput = CalcDamage(input);
	FDamageOutput damageOutput;
	damageOutput.DamageOutput = damagePreOutput;
	damageOutput.DamagedCharacter = this;

	if (IsInvincible())
	{
		FDamageOutput ignoredOutput = damageOutput;
		ignoredOutput.DamageOutput.Result = EDamageOutput::Ignored;
		ignoredOutput.DamageOutput.HealthDamage = 0.0f;
		ignoredOutput.DamageOutput.SteminaDamage = 0.0f;
		return ignoredOutput;
	}

	OnDamage(damageOutput);

	return damageOutput;
}

void AGodfallCharacterBase::SetHealth(float value, bool& onZeroHealth)
{
	float prevHealth = mHealth;
	mHealth = FMath::Clamp(value, 0.0f, mMaxHealth);
	if (prevHealth > 0.0f && mHealth <= 0.0f)
	{
		onZeroHealth = true;
	}
	else
	{
		onZeroHealth = false;
	}

	OnHealthChanged(prevHealth, mHealth);
}

void AGodfallCharacterBase::SetHealth(float value)
{
	bool temp;
	SetHealth(value, temp);
}

void AGodfallCharacterBase::SetStemina(float value, bool& onMaxStemina)
{
	float prevStemina = mStemina;
	mStemina = FMath::Clamp(value, 0.0f, mMaxStemina);
	if (prevStemina < mMaxStemina && mStemina >= mMaxStemina)
	{
		onMaxStemina = true;
	}
	else
	{
		onMaxStemina = false;
	}

	OnSteminaChanged(prevStemina, prevStemina);
}

void AGodfallCharacterBase::SetStemina(float value)
{
	bool temp;
	SetStemina(value, temp);
}

bool AGodfallCharacterBase::IsStun() const
{
	return mGodfallCharacterAnimInstance->IsPlayingStunMontage();
}

void AGodfallCharacterBase::InitSkeletalMesh(FString referencePath, FVector relativeLocation, FRotator relativeRotation)
{
	USkeletalMeshComponent* characterMesh = GetMesh();

	ConstructorHelpers::FObjectFinder<USkeletalMesh> skeletalMesh(*referencePath);
	if (ensure(skeletalMesh.Succeeded()))
	{
		characterMesh->SetSkeletalMesh(skeletalMesh.Object);
		characterMesh->SetRelativeLocationAndRotation(relativeLocation, relativeRotation);
	}
}

void AGodfallCharacterBase::InitAnimInstance(FString referencePath)
{
	USkeletalMeshComponent* characterMesh = GetMesh();

	ConstructorHelpers::FClassFinder<UAnimInstance> animInstance(*referencePath);
	if (ensure(animInstance.Succeeded()))
	{
		characterMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		characterMesh->SetAnimInstanceClass(animInstance.Class);
	}
}

void AGodfallCharacterBase::InitCapsule(float halfHeight, float radius)
{
	UCapsuleComponent* capsule = GetCapsuleComponent();

	capsule->InitCapsuleSize(radius, halfHeight);
}

void AGodfallCharacterBase::InitHitboxIndex(int index, UPrimitiveComponent* hitbox)
{
	if (!ensure(hitbox)) return;

	Hitbox->AddHitboxIndex(index, hitbox);
}
