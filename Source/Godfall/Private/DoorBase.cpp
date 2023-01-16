// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorBase.h"

// Sets default values
ADoorBase::ADoorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADoorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (UseDoorStateOverride)
	{
		switch (DoorStateOverride)
		{
			case EDoorStateOverride::Open:
			{
				Open();
			}
			break;
			case EDoorStateOverride::Close:
			{
				Close();
			}
			break;
		}
	}

	switch (DoorState)
	{
		case EDoorState::Opening:
		{
			OnOpening();
		}
		break;
		case EDoorState::Closing:
		{
			OnClosing();
		}
		break;
	}
}

void ADoorBase::Open()
{
	switch (DoorState)
	{
		case EDoorState::Open:
		case EDoorState::Opening:
			return;
	}

	DoorState = EDoorState::Opening;
	DoorBeginOpen();
}

void ADoorBase::Close()
{
	switch (DoorState)
	{
		case EDoorState::Close:
		case EDoorState::Closing:
			return;
	}

	DoorState = EDoorState::Closing;
	DoorBeginClose();
}

void ADoorBase::OpenImplementation()
{
	if (UseDoorStateOverride)
	{
		return;
	}
	Open();
}

void ADoorBase::CloseImplementation()
{
	if (UseDoorStateOverride)
	{
		return;
	}
	Close();
}

void ADoorBase::OpenDirect()
{
	DoorState = EDoorState::Open;
	OnOpenDirect();
}

void ADoorBase::ClosetDirect()
{
	DoorState = EDoorState::Close;
	OnCloseDirect();
}

void ADoorBase::SetToOpen()
{
	DoorState = EDoorState::Open;
	OnOpen();
}

void ADoorBase::SetToClose()
{
	DoorState = EDoorState::Close;
	OnClose();
}

