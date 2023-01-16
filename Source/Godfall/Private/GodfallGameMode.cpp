// Copyright Epic Games, Inc. All Rights Reserved.

#include "GodfallGameMode.h"
#include "GodfallCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "CPlayer.h"
#include "GodfallPlayerController.h"
#include "GodfallGameState.h"

AGodfallGameMode::AGodfallGameMode()
{
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}
	DefaultPawnClass = ACPlayer::StaticClass();
	PlayerControllerClass = AGodfallPlayerController::StaticClass();
	GameStateClass = AGodfallGameState::StaticClass();
}
