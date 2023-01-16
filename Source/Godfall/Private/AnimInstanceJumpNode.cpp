// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstanceJumpNode.h"

void UAnimInstanceJumpNode::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	USkeletalMeshComponent* mesh = GetOwningComponent();
	if (!ensure(mesh)) return;

	for (int i = 0; i < mesh->GetNumMaterials(); ++i)
	{
		UMaterialInstanceDynamic* material = mesh->CreateAndSetMaterialInstanceDynamic(i);
		if (!material) mMaterials.Add(material);
	}
}
