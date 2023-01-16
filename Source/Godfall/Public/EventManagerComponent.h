// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "Components/ActorComponent.h"
#include "EventManagerComponent.generated.h"

UINTERFACE(Blueprintable)
class UEventListenerInterface : public UInterface
{
	GENERATED_BODY()
};

class GODFALL_API IEventListenerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnEventListen(UObject* Notifier, const FName& Name, const FName& Context);

	// On C++
	// virtual void OnEventListen_Implementation(UObject* Notifier, const FName& Name, const FName& Context) override;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GODFALL_API UEventManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEventManagerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable)
	static bool IsEventListenerInterface(UObject* Object);

	static IEventListenerInterface* CastToEventListenerInterface(UObject* Object);

	UFUNCTION(BlueprintCallable)
	bool RegistListener(UObject* EventListenerInterface);

	UFUNCTION(BlueprintCallable)
	bool UnregistListener(UObject* EventListenerInterface);

	UFUNCTION(BlueprintCallable)
	bool IsRegistedListener(UObject* EventListenerInterface) const;

	UFUNCTION(BlueprintCallable)
	void NotifyEvent(UObject* Notifier, const FName& Name, const FName& Tag);

private:
	UPROPERTY(VisibleAnywhere, DisplayName = "Listeners", Meta = (AllowPrivateAccess))
	TArray<TWeakObjectPtr<UObject>> mListeners;
};
