// Fill out your copyright notice in the Description page of Project Settings.


#include "GodfallAnimNotifyBase.h"

UGodfallAnimNotifyBase::UGodfallAnimNotifyBase()
{
}

void UGodfallAnimNotifyBase::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	UAnimInstance* animInstance = MeshComp->GetAnimInstance();
	if (animInstance)
	{
		UFunction* function = animInstance->FindFunction(GetFunctionName());
		if (function && function->NumParms == 0)
		{
			FOnAnimNotifyCalled Delegate;
			Delegate.BindUFunction(animInstance, GetFunctionName());
			Delegate.Execute();
		}
	}
}
