// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GodfallAnimNotifyStateBase.generated.h"

UENUM(BlueprintType)
enum class EAnimNotifyStateEvent : uint8
{
	Begin,
	Tick,
	End,
};

USTRUCT(BlueprintType)
struct FAnimNotifiyStateData
{
	GENERATED_USTRUCT_BODY()

	USkeletalMeshComponent* MeshComp;
	UAnimSequenceBase* Animation;
	const FAnimNotifyEventReference* EventReference;
	float TotalDuration = 0; // Valid in Begin
	float FrameDeltaTime = 0; // Valid in Tick
};

class UGodfallAnimNotifyStateBase;
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnAnimNotifyStateEvent, EAnimNotifyStateEvent, eEvent, const UGodfallAnimNotifyStateBase*, notify, const FAnimNotifiyStateData&, data);

/**
 * 
 */
UCLASS(Abstract)
class GODFALL_API UGodfallAnimNotifyStateBase : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UGodfallAnimNotifyStateBase();

public:
	virtual FName GetFunctionName() const { return TEXT(""); }

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
