// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "GameFramework/Pawn.h"
#include "AShield.generated.h"

UCLASS()
class GODFALL_API AAShield : public APawn
{
	GENERATED_BODY()

public:
	AAShield();

	bool IsOpen() const;
	void SetOpen(bool value);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UFUNCTION()
	void OnShieldClosed();

	UFUNCTION()
	void OnShieldOpen();

	void SetShieldVisibility(bool value);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* ShieldMesh;
};
