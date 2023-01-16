// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_Footstep.h"
#include <Animation/AnimNotifies/AnimNotify_PlaySound.h>

UAnimNotify_Footstep::UAnimNotify_Footstep()
{
}

void UAnimNotify_Footstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
}
