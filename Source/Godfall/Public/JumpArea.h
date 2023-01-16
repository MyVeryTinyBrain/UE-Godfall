// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "GameFramework/Actor.h"
#include "JumpArea.generated.h"

class USplineComponent;

UENUM(BlueprintType)
enum class EJumpAreaType : uint8
{
	Jump,
	Jump_Travel,
};

UCLASS()
class GODFALL_API AJumpArea : public AActor
{
	GENERATED_BODY()
	
public:	
	AJumpArea();

protected:
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EditorTick(float DeltaTime);
	virtual void RuntimeTick(float DeltaTime);

private:
	UFUNCTION()
	void OnAreaBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAreaEndOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	bool PlayerIsOverlapping() const { return mPlayer.IsValid(); }
	bool PlayerIsOverlappingInArea0() const { return mOverlappingArea0; }
	bool PlayerIsOverlappingInArea1() const { return mOverlappingArea1; }

	bool GetParabolaCoefficient(struct FParabolaCoefficient3* out) const;
	EJumpAreaType GetJumpAreaType() const { return mJumpAreaType; }

protected:
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess))
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess))
	UBoxComponent* Area0;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess))
	UBoxComponent* Area1;

	UPROPERTY(EditAnywhere, DisplayName = "Jump Area Type", Category = "Actor", Meta = (AllowPrivateAccess))
	EJumpAreaType mJumpAreaType = EJumpAreaType::Jump;

	UPROPERTY(EditAnywhere, DisplayName = "Cross Point Height", Category = "Actor", Meta = (AllowPrivateAccess))
	float mCrossPointHeight = 100.0f;

	UPROPERTY(VisibleAnywhere, DisplayName = "Player", Category = "Actor", Meta = (AllowPrivateAccess))
	TWeakObjectPtr<ACharacter> mPlayer;

	UPROPERTY(VisibleAnywhere, DisplayName = "Overlapping Area0", Category = "Actor", Meta = (AllowPrivateAccess))
	bool mOverlappingArea0 = false;

	UPROPERTY(VisibleAnywhere, DisplayName = "Overlapping Area1", Category = "Actor", Meta = (AllowPrivateAccess))
	bool mOverlappingArea1 = false;
};
