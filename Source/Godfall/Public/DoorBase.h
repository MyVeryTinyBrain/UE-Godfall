// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "GameFramework/Actor.h"
#include "DoorBase.generated.h"

UENUM(BlueprintType)
enum class EDoorState : uint8
{
	Opening,
	Open,
	Closing,
	Close,
};

UENUM(BlueprintType)
enum class EDoorStateOverride : uint8
{
	Open,
	Close,
};

UCLASS()
class GODFALL_API ADoorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ADoorBase();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void Open();

	UFUNCTION(BlueprintCallable)
	virtual void Close();

	UFUNCTION(BlueprintCallable)
	virtual void OpenImplementation();

	UFUNCTION(BlueprintCallable)
	virtual void CloseImplementation();

	UFUNCTION(BlueprintCallable)
	virtual void OpenDirect();

	UFUNCTION(BlueprintCallable)
	virtual void ClosetDirect();

	EDoorState GetDoorState() const { return DoorState; }

protected:
	UFUNCTION(BlueprintCallable)
	void SetToOpen();

	UFUNCTION(BlueprintCallable)
	void SetToClose();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DoorBeginOpen();
	virtual void DoorBeginOpen_Implementation() {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DoorBeginClose();
	virtual void DoorBeginClose_Implementation() {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnOpening();
	virtual void OnOpening_Implementation() {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnOpen();
	virtual void OnOpen_Implementation() {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnOpenDirect();
	virtual void OnOpenDirect_Implementation() {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnClosing();
	virtual void OnClosing_Implementation() {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnClose();
	virtual void OnClose_Implementation() {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnCloseDirect();
	virtual void OnCloseDirect_Implementation() {};

protected:
	UPROPERTY(BlueprintReadWrite)
	EDoorState DoorState = EDoorState::Close;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Interp)
	bool UseDoorStateOverride = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Interp)
	EDoorStateOverride DoorStateOverride = EDoorStateOverride::Close;
};
