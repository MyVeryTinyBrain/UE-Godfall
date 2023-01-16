// Fill out your copyright notice in the Description page of Project Settings.


#include "EventManagerComponent.h"

UEventManagerComponent::UEventManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEventManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEventManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UEventManagerComponent::IsEventListenerInterface(UObject* Object)
{
	return Object->GetClass()->ImplementsInterface(UEventListenerInterface::StaticClass());
}

IEventListenerInterface* UEventManagerComponent::CastToEventListenerInterface(UObject* Object)
{
	if (!Object)
	{
		return nullptr;
	}

	if (!IsEventListenerInterface(Object))
	{
		return nullptr;
	}

	return (IEventListenerInterface*)Object;
}

bool UEventManagerComponent::RegistListener(UObject* EventListenerInterface)
{
	if (IsRegistedListener(EventListenerInterface)) return false;
	
	IEventListenerInterface* UObjectListener = CastToEventListenerInterface(EventListenerInterface);
	if (UObjectListener)
	{
		mListeners.Add(EventListenerInterface);
		return true;
	}

	return false;
}

bool UEventManagerComponent::UnregistListener(UObject* EventListenerInterface)
{
	IEventListenerInterface* UObjectListener = CastToEventListenerInterface(EventListenerInterface);
	if (UObjectListener)
	{
		return (mListeners.Remove(EventListenerInterface) > 0);
	}

	return false;
}

bool UEventManagerComponent::IsRegistedListener(UObject* EventListenerInterface) const
{
	IEventListenerInterface* UObjectListener = CastToEventListenerInterface(EventListenerInterface);
	if (!UObjectListener) return false;

	return mListeners.Contains(EventListenerInterface);
}

void UEventManagerComponent::NotifyEvent(UObject* Notifier, const FName& Name, const FName& Tag)
{
	TArray<TWeakObjectPtr<UObject>> invalidListeners;
	TArray<TWeakObjectPtr<UObject>> listeners = mListeners;
	for (auto& listener : listeners)
	{
		if (!listener.IsValid())
		{
			invalidListeners.Add(listener);
			continue;
		}
		else
		{
			IEventListenerInterface* UOBjectListener = (IEventListenerInterface*)listener.Get();
			IEventListenerInterface::Execute_OnEventListen(listener.Get(), Notifier, Name, Tag);
		}
	}

	for (auto& invalidListener : invalidListeners)
	{
		listeners.Remove(invalidListener);
	}
	mListeners = listeners;
}

