// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicPlayer.h"

TWeakObjectPtr<AMusicPlayer> AMusicPlayer::MusicPlayer = nullptr;

AMusicPlayer::AMusicPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMusicPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (MusicPlayer.IsValid())
	{
		MusicPlayer->Destroy();
	}

	MusicPlayer = this;
}

void AMusicPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMusicPlayer::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (MusicPlayer == this)
	{
		MusicPlayer = nullptr;
	}
}

AMusicPlayer* AMusicPlayer::GetMusicPlayer(UObject* WorldObject)
{
	if (!WorldObject) return nullptr;

	UWorld* world = WorldObject->GetWorld();
	if (!world) return nullptr;

	#if WITH_EDITOR
	if (world->IsEditorWorld() && world->WorldType != EWorldType::PIE)
	{
		return nullptr;
	}
	#endif

	if (MusicPlayer.IsValid())
	{
		return MusicPlayer.Get();
	}

	return world->SpawnActor<AMusicPlayer>(AMusicPlayer::StaticClass());
}

void AMusicPlayer::Play(USoundBase* Sound, float volume, float FadeOutTime)
{
	if (!Sound) return;

	Stop(FadeOutTime);

	PlayingAudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), Sound, volume, 1.0f, 0.0f);
}

void AMusicPlayer::Stop(float FadeOutTime)
{
	if (PlayingAudioComponent.IsValid())
	{
		PlayingAudioComponent->FadeOut(FadeOutTime, 0.0f);
		PlayingAudioComponent = nullptr;
	}
}

