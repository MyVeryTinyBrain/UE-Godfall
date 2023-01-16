// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "GameFramework/Actor.h"
#include "EventManagerComponent.h"
#include "MusicPlayer.generated.h"

UCLASS()
class GODFALL_API AMusicPlayer : public AActor
{
	GENERATED_BODY()
	
public:	
	AMusicPlayer();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintCallable)
	static AMusicPlayer* GetMusicPlayer(UObject* WorldObject);

	UFUNCTION(BlueprintCallable)
	void Play(USoundBase* Sound, float volume = 1.0f, float FadeOutTime = 0.0f);

	UFUNCTION(BlueprintCallable)
	void Stop(float FadeOutTime = 0.0f);

private:
	UPROPERTY()
	TWeakObjectPtr<UAudioComponent> PlayingAudioComponent = nullptr;

	static TWeakObjectPtr<AMusicPlayer> MusicPlayer;
};
