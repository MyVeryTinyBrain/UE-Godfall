// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "GameFramework/GameStateBase.h"
#include "GodfallGameState.generated.h"

class UCharacterManagerComponent;
class UEventManagerComponent;

/**
 * 
 */
UCLASS()
class GODFALL_API AGodfallGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AGodfallGameState();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	UCharacterManagerComponent* GetCharacterManager() const { return CharacterManager; }

	UFUNCTION(BlueprintCallable)
	UEventManagerComponent* GetEventManager() const { return EventManager; }

private:
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess))
	UCharacterManagerComponent* CharacterManager;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess))
	UEventManagerComponent* EventManager;
};
