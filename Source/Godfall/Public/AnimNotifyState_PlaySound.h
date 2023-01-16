// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallAnimNotifyStateBase.h"
#include "AnimNotifyState_PlaySound.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "PlaySound")
class GODFALL_API UAnimNotifyState_PlaySound : public UGodfallAnimNotifyStateBase
{
	GENERATED_BODY()
	
public:
	UAnimNotifyState_PlaySound();

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:
	virtual FString GetNotifyName_Implementation() const override;
	virtual FName GetFunctionName() const override { return TEXT("AnimNotifyState_PlaySound"); };

	#if WITH_EDITOR
	virtual void ValidateAssociatedAssets() override;
	#endif

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotifyState", meta = (ExposeOnSpawn = true))
	TObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotifyState", meta = (ExposeOnSpawn = true))
	float InterruptFadeOutTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotifyState", meta = (ExposeOnSpawn = true))
	float InterruptFadeVolumeLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotifyState", meta = (ExposeOnSpawn = true))
	float VolumeMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotifyState", meta = (ExposeOnSpawn = true))
	float PitchMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotifyState", meta = (ExposeOnSpawn = true))
	float StartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotifyState")
	uint32 bFollow : 1;

	#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "AnimNotifyState")
	uint32 bPreviewIgnoreAttenuation : 1;
	#endif

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotifyState", meta = (EditCondition = "bFollow", ExposeOnSpawn = true))
	FName AttachName;

	UPROPERTY()
	TWeakObjectPtr<UAudioComponent> AudioComponent;

	#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TWeakObjectPtr<UAudioComponent> PreviewAudioComponent;
	#endif
};
