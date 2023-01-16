// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "GameFramework/PlayerController.h"
#include "GodfallPlayerController.generated.h"

class UPlayerHUD;

/**
 * 
 */
UCLASS()
class GODFALL_API AGodfallPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AGodfallPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	UPlayerHUD* GetPlayerHUD() const { return PlayerHUD; }

private:
	UPROPERTY()
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	UPROPERTY()
	UPlayerHUD* PlayerHUD = nullptr;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Interp)
	bool ShowHUD = true;
};
