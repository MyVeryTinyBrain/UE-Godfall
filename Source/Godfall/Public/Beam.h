// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "GameFramework/Actor.h"
#include "DamageTypes.h"
#include "Beam.generated.h"

UCLASS()
class GODFALL_API ABeam : public AActor
{
	GENERATED_BODY()
	
public:	
	ABeam();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHitboxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

public:
	FVector GetBeamMaxEndPoint() const;
	bool IsDamagableActor(AActor* actor) const;

	void SetActiveDirect(bool value);
	void SetActive(bool value);
	bool GetActive() const;

	void UpdateBeam();

private:
	void UpdateScale(float deltaTime);

private:
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess))
	UNiagaraComponent* BeamEffect;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess))
	UCapsuleComponent* BeamCollision;

public:
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess))
	UMaterial* BeamDecalMaterial;

public:
	UPROPERTY(EditAnywhere, DisplayName = "Ignore Actors", Category = "Actor", Meta = (AllowPrivateAccess))
	TArray<AActor*> mIgnoreActors;

	UPROPERTY(EditAnywhere, DisplayName = "Use Collision Tags", Category = "Actor", Meta = (AllowPrivateAccess))
	bool mUseCollisionTags = false;

	UPROPERTY(EditAnywhere, DisplayName = "Collision Tags", Category = "Actor", Meta = (AllowPrivateAccess))
	TArray<FName> mCollisionTags;

	UPROPERTY(EditAnywhere, DisplayName = "Beam Width", Category = "Actor", Meta = (AllowPrivateAccess, ClampMin = 0.0f))
	float mBeamWidth = 10.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Beam Width Scale", Category = "Actor", Meta = (AllowPrivateAccess, ClampMin = 0.0f, ClampMax = 1.0f))
	float mBeamWidthScale = 1.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Beam Hit Width", Category = "Actor", Meta = (AllowPrivateAccess, ClampMin = 0.0f))
	float mBeamHitWidth = 20.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Beam Max Length", Category = "Actor", Meta = (AllowPrivateAccess, ClampMin = 0.0f))
	float mBeamMaxLength = 10000.0f;

private:
	UPROPERTY(VisibleAnywhere, DisplayName = "Beam Active", Category = "Actor", Meta = (AllowPrivateAccess))
	bool mBeamActive = true;

	UPROPERTY(VisibleAnywhere, DisplayName = "Beam Scale", Category = "Actor", Meta = (AllowPrivateAccess))
	float mBeamScale = 1.0f;

	bool mReady = true;
};
