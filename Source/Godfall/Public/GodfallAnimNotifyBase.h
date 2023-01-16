// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GodfallAnimNotifyBase.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnAnimNotifyCalled);

/**
 * 
 */
UCLASS(Abstract)
class GODFALL_API UGodfallAnimNotifyBase : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UGodfallAnimNotifyBase();

public:
	virtual FName GetFunctionName() const { return TEXT(""); }
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
