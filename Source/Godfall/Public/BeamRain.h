// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "GameFramework/Actor.h"
#include "BeamRain.generated.h"

UENUM(BlueprintType)
enum class EBeamRainState : uint8
{
	Delay,
	IncreaseBeamScale,
	Working,
	DecreaseBeamScale,
};

UCLASS()
class GODFALL_API ABeamRain : public AActor
{
	GENERATED_BODY()
	
public:	
	ABeamRain();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHitboxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

public:
	bool IsDamagableActor(AActor* actor) const;

public:
	static ABeamRain* SpawnBeamRain(UWorld* world, const FVector& location, bool raycast);

private:
	void UpdateBeam();
	void UpdateImpact();

private:
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess))
	UNiagaraComponent* AreaEffect;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess))
	UNiagaraComponent* ImpactEffect;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess))
	USphereComponent* ImpactCollision;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess))
	UNiagaraComponent* BeamEffect;

public:
	UPROPERTY(EditAnywhere, DisplayName = "Ignore Actors", Category = "Actor", Meta = (AllowPrivateAccess))
	TArray<AActor*> mIgnoreActors;

	UPROPERTY(EditAnywhere, DisplayName = "Use Collision Tags", Category = "Actor", Meta = (AllowPrivateAccess))
	bool mUseCollisionTags = false;

	UPROPERTY(EditAnywhere, DisplayName = "Collision Tags", Category = "Actor", Meta = (AllowPrivateAccess))
	TArray<FName> mCollisionTags;

	UPROPERTY(EditAnywhere, DisplayName = "Impact Radius", Category = "Actor", Meta = (AllowPrivateAccess, ClampMin = 0.0f))
	float mImpactRadius = 200.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Impact Delay", Category = "Actor", Meta = (AllowPrivateAccess, ClampMin = 0.0f))
	float mImpactDelay = 0.5f;

	UPROPERTY(EditAnywhere, DisplayName = "Impact Delay Fadeout Time", Category = "Actor", Meta = (AllowPrivateAccess, ClampMin = 0.0f))
	float mImpactDelayFadeoutTime = 0.2f;

	UPROPERTY(EditAnywhere, DisplayName = "Impact Duration", Category = "Actor", Meta = (AllowPrivateAccess, ClampMin = 0.0f))
	float mImpactDuration = 2.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Beam Width", Category = "Actor", Meta = (AllowPrivateAccess, ClampMin = 0.0f))
	float mBeamWidth = 50.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Beam Length", Category = "Actor", Meta = (AllowPrivateAccess, ClampMin = 0.0f))
	float mBeamLength = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "Impact Sound", Category = "Actor", meta = (ExposeOnSpawn = true))
	TObjectPtr<USoundBase> mImpactSound;

private:
	UPROPERTY(VisibleAnywhere, DisplayName = "State", Category = "Actor", Meta = (AllowPrivateAccess))
	EBeamRainState mState = EBeamRainState::Delay;

	UPROPERTY(VisibleAnywhere, DisplayName = "Accumulation", Category = "Actor", Meta = (AllowPrivateAccess))
	float mAccumulation = 0.0f;

	UPROPERTY(VisibleAnywhere, DisplayName = "Effect Alpha Scale", Category = "Actor", Meta = (AllowPrivateAccess))
	float mEffectAlphaScale = 0.0f;
};
