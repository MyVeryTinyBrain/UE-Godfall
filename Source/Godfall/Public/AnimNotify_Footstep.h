// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallAnimNotifyBase.h"
#include "AnimNotify_Footstep.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Footstep")
class GODFALL_API UAnimNotify_Footstep : public UGodfallAnimNotifyBase
{
	GENERATED_BODY()
	
public:
	UAnimNotify_Footstep();

public:
	virtual FName GetFunctionName() const override { return TEXT("AnimNotify_Footstep"); }
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
