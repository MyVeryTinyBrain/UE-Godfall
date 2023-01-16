// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GodfallAnimInstanceBase.h"
#include "AnimInstanceJumpNode.generated.h"

/**
 * 
 */
UCLASS()
class GODFALL_API UAnimInstanceJumpNode : public UGodfallAnimInstanceBase
{
	GENERATED_BODY()

protected:
	virtual void NativeBeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true), DisplayName = "Node Active")
	bool mNodeActive = false;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> mMaterials;
};
