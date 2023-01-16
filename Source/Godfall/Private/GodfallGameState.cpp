// Fill out your copyright notice in the Description page of Project Settings.


#include "GodfallGameState.h"
#include "CharacterManagerComponent.h"
#include "EventManagerComponent.h"

AGodfallGameState::AGodfallGameState()
{
	CharacterManager = CreateDefaultSubobject<UCharacterManagerComponent>(TEXT("Character Manager"));
	EventManager = CreateDefaultSubobject<UEventManagerComponent>(TEXT("Event Manager"));
}

void AGodfallGameState::BeginPlay()
{
	Super::BeginPlay();
}
