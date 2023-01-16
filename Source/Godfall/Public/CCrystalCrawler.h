// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallEnemyBase.h"
#include "CCrystalCrawler.generated.h"

/**
 * 
 */
UCLASS()
class GODFALL_API ACCrystalCrawler : public AGodfallEnemyBase
{
	GENERATED_BODY()
	
public:
	ACCrystalCrawler();

protected:
	virtual void PostInitializeComponents() override;
	virtual void SetupStateTable(FStateTable& mStateTable) override;
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

private:
	UPROPERTY()
	class UAnimInstanceCrystalCrawler* mAnimInstance = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	USphereComponent* HeadHitbox;
};
