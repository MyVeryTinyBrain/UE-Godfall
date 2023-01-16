// Fill out your copyright notice in the Description page of Project Settings.


#include "GodfallPlayerController.h"
#include "PlayerHUD.h"

AGodfallPlayerController::AGodfallPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UPlayerHUD> playerHUD(TEXT("/Game/UI/PlayerHUD.PlayerHUD_C"));
	if (ensure(playerHUD.Succeeded()))
	{
		PlayerHUDClass = playerHUD.Class;
	}
}

void AGodfallPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
}

void AGodfallPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerHUD = CreateWidget<UPlayerHUD>(this, PlayerHUDClass);
	PlayerHUD->AddToViewport();
}

void AGodfallPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Automatically set visible of HUD
	if (ensure(PlayerHUD))
	{
		if (ShowHUD)
		{
			if(PlayerHUD->GetVisibility() != ESlateVisibility::Visible)
			{
				PlayerHUD->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else
		{
			if (PlayerHUD->GetVisibility() != ESlateVisibility::Hidden)
			{
				PlayerHUD->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}
