// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "DamageTypes.h"
#include "GodfallAnimNotifyStateBase.h"
#include "GodfallAnimInstanceBase.h"
#include "GameFramework/Character.h"
#include "RotateTypes.h"
#include "GodfallCharacterBase.generated.h"


class UGodfallAnimInstanceBase;
class UGodfallCharacterAnimInstanceBase;
class UCharacterRotateComponent;
class UHitboxComponent;

UENUM(BlueprintType)
enum class ECharacterDamagableType : uint8
{
	/* 모든 데미지 동작을 수행합니다. */
	AllDamage,
	
	/* 애디티브 데미지 동작만을 수행합니다. */
	AdditiveOnly,
	
	/* 데미지 동작을 수행하지 않습니다. */
	NoDamage,
};

UCLASS()
class GODFALL_API AGodfallCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AGodfallCharacterBase();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	virtual void OnDestroyedCallback(AActor* destroyed);

	UFUNCTION()
	virtual void OnDamagableEvent(EAnimNotifyStateEvent eEvent, const FDamagableData& data);

	UFUNCTION()
	virtual void OnRotateEvent(EAnimNotifyStateEvent eEvent, const FRotateOverride& data);

	UFUNCTION()
	virtual void OnAnimNotifyFootstep();

	virtual bool CanRotate() const { return true; }

	virtual bool IsInvincible() const { return false; }

	UFUNCTION()
	void OnHitboxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	FDamageInput OnHit(const FDamagableInput& damagableData);
	virtual EBlockOutput CanBlock(const FDamageInput& damageInput) const { return EBlockOutput::Fail; }
	FDamagePreOutput CalcDamage(const FDamageInput& input) const;
	void OnDamage(const FDamageOutput& output);
	void PlayDamageMontage(const FDamageOutput& damageOutput);
	void PlayAdditiveDamageMontage(const FDamageOutput& damageOutput);
	void OnPostHit(const FDamageOutput& damageOutput);

	virtual void PlayDamageSound(const FDamageOutput& output);
	void PlayFootstepSound();

	virtual void OnDamageImpl(const FDamageOutput& output, bool onZeroHealth, bool onMaxStemina) {}
	virtual void OnGuard(const FDamageOutput& damageOutput) {}
	virtual void OnZeroHealth() {}
	virtual void OnMaxStemina() {}
	virtual void OnHealthChanged(float prev, float changed) {}
	virtual void OnSteminaChanged(float prev, float changed) {}

	virtual void OnBeginStun() {}
	virtual void OnEndStun(bool interrupted);
	virtual void OnEndDeath();

	void ToDead();
	virtual void OnDead();

public:
	//virtual bool IsSuperarmor() const { return false; }
	FDamageOutput Damage(const FDamageInput& input);

	float GetMaxHealth() const { return mMaxHealth; }
	float GetHealth() const { return mHealth; }
	float GetMaxStemina() const { return mMaxStemina; }
	float GetStemina() const { return mStemina; }

	void SetHealth(float value, bool& onZeroHealth);
	void SetHealth(float value);
	void SetStemina(float value, bool& onMaxStemina);
	void SetStemina(float value);

	bool IsStun() const;

	UGodfallAnimInstanceBase* GetGodfallAnimInstance() const { return mGodfallAnimInstance; }
	UGodfallCharacterAnimInstanceBase* GetGodfallCharacterAnimInstance() const { return mGodfallCharacterAnimInstance; }
	UCharacterRotateComponent* GetCharacterRotateComponent() const { return CharacterRotate; }
	UHitboxComponent* GetHitboxComponent() const { return Hitbox; }

protected:
	void InitSkeletalMesh(FString referencePath, FVector relativeLocation = FVector::ZeroVector, FRotator relativeRotation = FRotator(0.0f, -90.0f, 0.0f));
	void InitAnimInstance(FString referencePath);
	void InitCapsule(float halfHeight, float radius);
	void InitHitboxIndex(int index, UPrimitiveComponent* hitbox);

protected:
	UPROPERTY(EditAnywhere, DisplayName = "Set Health from Max Health", Category = "Actor", Meta = (AllowPrivateAccess = true))
	bool mSetHealthFromMaxHealth = true;

	UPROPERTY(EditAnywhere, DisplayName = "Max Health", Category = "Actor", Meta = (AllowPrivateAccess = true, ClampMin = 0.0))
	float mMaxHealth = 10.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Max Stemina", Category = "Actor", Meta = (AllowPrivateAccess = true, ClampMin = 0.0))
	float mMaxStemina = 10.0f;

	bool mDead = false;

private:
	UPROPERTY(EditAnywhere, DisplayName = "Health", Category = "Actor", Meta = (AllowPrivateAccess = true, ClampMin = 0.0))
	float mHealth = 0.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Stemina", Category = "Actor", Meta = (AllowPrivateAccess = true, ClampMin = 0.0))
	float mStemina = 0.0f;

public:
	UPROPERTY(EditAnywhere, DisplayName = "Health Damage", Category = "Actor", Meta = (AllowPrivateAccess = true, ClampMin = 0.0))
	float mHealthDamage = 1.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Stemina Damage", Category = "Actor", Meta = (AllowPrivateAccess = true, ClampMin = 0.0))
	float mSteminaDamage = 1.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Block Stemina Damage", Category = "Actor", Meta = (AllowPrivateAccess = true, ClampMin = 0.0))
	float mBlockSteminaDamage = 10.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Use Stun kill", Category = "Actor", Meta = (AllowPrivateAccess = true))
	bool mUseStunKill = false; // Get max damage when stun

	UPROPERTY(EditAnywhere, DisplayName = "Damagable Type", Category = "Actor", Meta = (AllowPrivateAccess = true))
	ECharacterDamagableType mDamagableType = ECharacterDamagableType::AllDamage;

	UPROPERTY()
	UGodfallAnimInstanceBase* mGodfallAnimInstance = nullptr;

	UPROPERTY()
	UGodfallCharacterAnimInstanceBase* mGodfallCharacterAnimInstance = nullptr;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Damage Sound", Category = "Sound", meta = (ExposeOnSpawn = true))
	TObjectPtr<USoundBase> mDamageSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Footstep Sound", Category = "Sound", meta = (ExposeOnSpawn = true))
	TObjectPtr<USoundBase> mFootstepSound;

private:
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess))
	UCharacterRotateComponent* CharacterRotate = nullptr;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess))
	UHitboxComponent* Hitbox = nullptr;

	FDamagableData mDamagableData;

private:
	UPROPERTY()
	UNiagaraSystem* mHitEffect;
};
