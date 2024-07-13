// Fill out your copyright notice in the Description page of Project Settings.


#include "HitboxComponent.h"
#include "GodfallUtil.h"

// Sets default values for this component's properties
UHitboxComponent::UHitboxComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHitboxComponent::BeginPlay()
{
	Super::BeginPlay();

	// Turn off all hitboxes
	for (const auto& index : mIndices)
	{
		SetHitboxCollision(index, false);
	}
}


// Called every frame
void UHitboxComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHitboxComponent::OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnOverlapWithoutCondition.Broadcast(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (!mUseCollisionTags)
	{
		OnOverlap.Broadcast(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	}
	else if(mUseCollisionTags)
	{
		if (CompareTags(OtherActor))
		{
			OnOverlap.Broadcast(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
		}
	}
}

void UHitboxComponent::RemoveHitbox(UPrimitiveComponent* hitbox)
{
	if (!ensure(hitbox)) return;

	TArray<int> emptyIndices;

	for (auto& it : mHitboxesByIndex)
	{
		int numRemoved = it.Value.Remove(hitbox);
		if (numRemoved > 0 && it.Value.IsEmpty())
		{
			emptyIndices.Add(it.Key);
		}
	}

	for (auto& emptyIndex : emptyIndices)
	{
		mHitboxesByIndex.Remove(emptyIndex);
		mHitBuffer.Remove(emptyIndex);
		mIndices.Remove(emptyIndex);
	}

	mHitboxes.Remove(hitbox);
	mHitboxIndices.Remove(hitbox);

	hitbox->OnComponentBeginOverlap.RemoveDynamic(this, &UHitboxComponent::OnHitboxOverlap);
}

void UHitboxComponent::AddHitboxIndex(int index, UPrimitiveComponent* hitbox)
{
	if (!ensure(hitbox)) return;
	
	RemoveHitbox(hitbox);

	TArray<UPrimitiveComponent*>& arrayByIndex = mHitboxesByIndex.FindOrAdd(index);
	arrayByIndex.Add(hitbox);

	mHitboxIndices.FindOrAdd(hitbox, index);
	mHitboxIndices[hitbox] = index;

	if (!mHitboxes.Contains(hitbox))
	{
		mHitboxes.Add(hitbox);
	}

	if (!mHitBuffer.Contains(index))
	{
		mHitBuffer.Add(index);
	}

	if (!mIndices.Contains(index))
	{
		mIndices.Add(index);
	}

	hitbox->SetCollisionProfileName(GodfallPresets::HitTrigger);
	hitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	hitbox->IgnoreActorWhenMoving(GetOwner(), true);

	hitbox->OnComponentBeginOverlap.AddDynamic(this, &UHitboxComponent::OnHitboxOverlap);
}

int UHitboxComponent::GetHitboxIndex(UPrimitiveComponent* hitbox) const
{
	const int* pIndex = mHitboxIndices.Find(hitbox);
	return pIndex ? (*pIndex) : (-1);
}

void UHitboxComponent::SetHitboxCollision(int index, bool value)
{
	TArray<UPrimitiveComponent*>* arrayByIndex = mHitboxesByIndex.Find(index);
	if (!ensure(arrayByIndex)) return;

	ECollisionEnabled::Type collisionEnabled = value ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;
	for (auto& it : *arrayByIndex)
	{
		it->SetCollisionEnabled(collisionEnabled);
	}
}

bool UHitboxComponent::AddActorToHitBuffer(int index, AActor* actor)
{
	if (!ensure(mHitBuffer.Contains(index))) return false;
	bool isAlreadyInSet = false;
	// index 번째의 무기 히트박스 셋에 공격한 액터를 추가합니다.
	// 이미 추가되어 있다면 공격은 유효하지 않습니다.
	// 이 셋은 Damagable 애니메이션 노티파이 스테이트를 벗어날 때 초기화됩니다.
	mHitBuffer[index].Add(actor, &isAlreadyInSet);
	return (!isAlreadyInSet);
}

bool UHitboxComponent::IsContainsInHitBuffer(int index, AActor* actor) const
{
	if (!ensure(mHitBuffer.Contains(index))) return false;
	return mHitBuffer[index].Contains(actor);
}

void UHitboxComponent::ResetHitBuffer(int index)
{
	for (auto& it : mHitBuffer)
	{
		it.Value.Empty();
	}
}

void UHitboxComponent::AddCollisionTag(const FName& tag)
{
	if (!mCollisionTags.Contains(tag))
	{
		mCollisionTags.Add(tag);
	}
}

void UHitboxComponent::SetCollisionTags(const TArray<FName>& tags)
{
	mCollisionTags = tags;
}

bool UHitboxComponent::RemoveCollisionTag(const FName& tag)
{
	return (mCollisionTags.Remove(tag) > 0);
}

bool UHitboxComponent::ContainsCollisionTag(const FName& tag)
{
	return mCollisionTags.Contains(tag);
}

bool UHitboxComponent::CompareTags(const AActor* otherActor) const
{
	return GodfallUtil::String::CompareNames(otherActor->Tags, mCollisionTags);
}

