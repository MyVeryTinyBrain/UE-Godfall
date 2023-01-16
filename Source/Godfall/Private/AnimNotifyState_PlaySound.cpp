// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_PlaySound.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundWaveProcedural.h"
#include "Animation/AnimSequenceBase.h"

#if WITH_EDITOR
#include "Logging/MessageLog.h"
#include "Misc/UObjectToken.h"
#endif

UAnimNotifyState_PlaySound::UAnimNotifyState_PlaySound()
{
	InterruptFadeOutTime = 0.2f;
	InterruptFadeVolumeLevel = 0.0f;
	VolumeMultiplier = 1.f;
	PitchMultiplier = 1.f;
	StartTime = 0.f;

	#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(196, 142, 255, 255);
	bPreviewIgnoreAttenuation = false;
	#endif // WITH_EDITORONLY_DATA

	bIsNativeBranchingPoint = true;
}

void UAnimNotifyState_PlaySound::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (Sound && MeshComp)
	{
		if (!Sound->IsOneShot())
		{
			UE_LOG(LogAudio, Warning, TEXT("PlaySound notify: Anim %s tried to play a sound asset which is not a one-shot: '%s'. Spawning suppressed."), *GetNameSafe(Animation), *GetNameSafe(Sound));
			return;
		}

		#if WITH_EDITORONLY_DATA
		UWorld* World = MeshComp->GetWorld();
		if (bPreviewIgnoreAttenuation && World && World->WorldType == EWorldType::EditorPreview)
		{
			PreviewAudioComponent = UGameplayStatics::SpawnSound2D(World, Sound, VolumeMultiplier, PitchMultiplier);
		}
		else
		#endif
		{
			if (bFollow)
			{
				AudioComponent = UGameplayStatics::SpawnSoundAttached(Sound, MeshComp, AttachName, FVector(ForceInit), EAttachLocation::SnapToTarget, false, VolumeMultiplier, PitchMultiplier, StartTime);
			}
			else
			{
				AudioComponent = UGameplayStatics::SpawnSoundAtLocation(MeshComp->GetWorld(), Sound, MeshComp->GetComponentLocation(), FRotator::ZeroRotator, VolumeMultiplier, PitchMultiplier, StartTime);
			}
		}
	}
}

void UAnimNotifyState_PlaySound::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UAnimNotifyState_PlaySound::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	#if WITH_EDITORONLY_DATA
	if (PreviewAudioComponent.IsValid())
	{
		PreviewAudioComponent->FadeOut(InterruptFadeOutTime, InterruptFadeVolumeLevel);
		//PreviewAudioComponent->Stop();
		PreviewAudioComponent = nullptr;
	}
	#endif

	if (AudioComponent.IsValid())
	{
		AudioComponent->FadeOut(InterruptFadeOutTime, InterruptFadeVolumeLevel);
		//AudioComponent->Stop();
		AudioComponent = nullptr;
	}
}

FString UAnimNotifyState_PlaySound::GetNotifyName_Implementation() const
{
	if (Sound)
	{
		return Sound->GetName();
	}
	else
	{
		return Super::GetNotifyName_Implementation();
	}
}

#if WITH_EDITOR
void UAnimNotifyState_PlaySound::ValidateAssociatedAssets()
{
	static const FName NAME_AssetCheck("AssetCheck");

	if (Sound != nullptr && !Sound->IsOneShot())
	{
		UObject* ContainingAsset = GetContainingAsset();

		FMessageLog AssetCheckLog(NAME_AssetCheck);

		const FText MessageLooping = FText::Format(
			NSLOCTEXT("AnimNotifyState", "Sound_ShouldNotLoop", "Sound {0} used in anim notify for asset {1} is set to looping, but the slot is a one-shot (it won't be played to avoid leaking an instance per notify)."),
			FText::AsCultureInvariant(Sound->GetPathName()),
			FText::AsCultureInvariant(ContainingAsset->GetPathName()));
		AssetCheckLog.Warning()
			->AddToken(FUObjectToken::Create(ContainingAsset))
			->AddToken(FTextToken::Create(MessageLooping));

		if (GIsEditor)
		{
			AssetCheckLog.Notify(MessageLooping, EMessageSeverity::Warning, /*bForce=*/ true);
		}
	}
}
#endif
