// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "Components/ActorComponent.h"
#include "HitboxComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(
FOnHitboxOverlap,
UPrimitiveComponent*, OverlappedComponent,
AActor*, OtherActor, UPrimitiveComponent*, OtherComp, int32, OtherBodyIndex,
bool, bFromSweep, const FHitResult&, SweepResult);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GODFALL_API UHitboxComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHitboxComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	

	UFUNCTION()
	void OnHitboxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

public:
	void RemoveHitbox(UPrimitiveComponent* hitbox);
	void AddHitboxIndex(int index, UPrimitiveComponent* hitbox);
	int GetHitboxIndex(UPrimitiveComponent* hitbox) const;
	void SetHitboxCollision(int index, bool value);
	bool AddActorToHitBuffer(int index, AActor* actor);
	bool IsContainsInHitBuffer(int index, AActor* actor) const;
	void ResetHitBuffer(int index);

	void SetCollisionTagUse(bool value) { mUseCollisionTags = value; }
	bool IsUsingCollisionTag()const { return mUseCollisionTags; }
	void AddCollisionTag(const FName& tag);
	void SetCollisionTags(const TArray<FName>& tags);
	bool RemoveCollisionTag(const FName& tag);
	bool ContainsCollisionTag(const FName& tag);

	bool CompareTags(const AActor* otherActor) const;

private: 
	// 등록된 모든 인덱스들입니다.
	TArray<int> mIndices;

	// 등록된 모든 히트박스들입니다.
	TArray<UPrimitiveComponent*> mHitboxes;

	// 등록된 히트박스의 인덱스가 나열되어 있습니다.
	// 같은 인덱스의 히트박스들은 하나의 히트박스로 취급합니다.
	// ex) 0번 인덱스의 히트박스A가 히트한 경우 0번 인덱스의 히트박스B가 히트하더라도 이벤트가 발생하지 않습니다.
	TMap<UPrimitiveComponent*, int> mHitboxIndices;

	// 인덱스로 히트박스들이 분류되어 있습니다.
	TMap<int, TArray<UPrimitiveComponent*>> mHitboxesByIndex;

	// 히트 버퍼입니다.
	// 히트된 액터들이 저장되며 리셋 전까지 저장됩니다.
	TMap<int, TSet<AActor*>> mHitBuffer;

private:
	UPROPERTY(EditAnywhere, DisplayName = "Use Collision Tags", Meta = (AllowPrivateAccess))
	bool mUseCollisionTags = false;

	UPROPERTY(EditAnywhere, DisplayName = "Collision Tags", Meta = (AllowPrivateAccess))
	TArray<FName> mCollisionTags;

public:
	FOnHitboxOverlap OnOverlap;
	FOnHitboxOverlap OnOverlapWithoutCondition;
};
