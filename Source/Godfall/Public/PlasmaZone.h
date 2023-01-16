// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "GameFramework/Actor.h"
#include "PlasmaZone.generated.h"

UENUM(BlueprintType)
enum class EPlasmaZoneState : uint8
{
	Working,
	Destorying,
};

UCLASS()
class GODFALL_API APlasmaZone : public AActor
{
	GENERATED_BODY()
	
public:	
	APlasmaZone();

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

	void UpdatePlasmaZone();

public:
	static APlasmaZone* SpawnPlasmaZone(UWorld* world, const FVector& location, bool raycast);

private:
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess))
	UNiagaraComponent* PlasmaZoneEffect;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess))
	USphereComponent* PlasmaZoneCollision;

public:
	UPROPERTY(EditAnywhere, DisplayName = "Ignore Actors", Category = "Actor", Meta = (AllowPrivateAccess))
	TArray<AActor*> mIgnoreActors;

	UPROPERTY(EditAnywhere, DisplayName = "Use Collision Tags", Category = "Actor", Meta = (AllowPrivateAccess))
	bool mUseCollisionTags = false;

	UPROPERTY(EditAnywhere, DisplayName = "Collision Tags", Category = "Actor", Meta = (AllowPrivateAccess))
	TArray<FName> mCollisionTags;

	UPROPERTY(EditAnywhere, DisplayName = "Plasma Zone Radius", Category = "Actor", Meta = (AllowPrivateAccess, ClampMin = 0.0f))
	float mPlasmaZoneRadius = 200.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Plasma Zone Duration", Category = "Actor", Meta = (AllowPrivateAccess, ClampMin = 0.0f))
	float mPlasmaZoneDuration = 2.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Plasma Zone Fade Out Time", Category = "Actor", Meta = (AllowPrivateAccess, ClampMin = 0.1f))
	float mPlasmaZoneFadeOutTime = 1.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Color", Category = "Actor", Meta = (AllowPrivateAccess))
	FLinearColor mColor = FLinearColor(1.0f, 0.064177, 0.0f, 1.0f);

	UPROPERTY(EditAnywhere, DisplayName = "Alpha", Category = "Actor", Meta = (AllowPrivateAccess, ClampMin = 0.0f))
	float mAlpha = 1.0f;

private:
	UPROPERTY(VisibleAnywhere, DisplayName = "Accumulation", Category = "Actor", Meta = (AllowPrivateAccess))
	float mAccumulation = 0.0f;

	UPROPERTY(VisibleAnywhere, DisplayName = "Alpha Scle", Category = "Actor", Meta = (AllowPrivateAccess))
	float mAlphaScale = 1.0f;

	UPROPERTY(VisibleAnywhere, DisplayName = "Plasma Zone State", Category = "Actor", Meta = (AllowPrivateAccess))
	EPlasmaZoneState mState = EPlasmaZoneState::Working;

	bool mReady = true;
};
