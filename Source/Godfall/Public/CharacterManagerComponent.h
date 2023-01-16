// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "Components/ActorComponent.h"
#include "CharacterManagerComponent.generated.h"

class AGodfallCharacterBase;
class AGodfallEnemyBase;

enum class ECharacterManagerQueryType : uint8
{
	None,
	Character,
	Enemy,
};

enum class ECharacterManagerQueryDistancePriority : uint8
{
	None,	// 거리 검사를 하지 않습니다.
	Near,	// 가장 가까운 것을 반환합니다.
	Far,	// 가장 먼 것을 반환합니다.
};

enum class ECharacterManagerQueryDistanceFunction : uint8
{
	/* 점과 점 사이의 거리
	Point에 점의 위치를 저장하세요. */
	PointToPoint,

	/* 선과 점 사이의 거리
	Point에 선의 시작 위치를 저장하세요.  
	Rotation에 선의 방향을 저장하세요. */
	PointToLine,

	/* 점과 점 사이의 거리, 선과 점 사이의 거리 중 짧은 거리
	Point에 선의 시작 위치를 저장하세요.  
	Rotation에 선의 방향을 저장하세요. */
	PointToPoint_PointToLine_Smaller,

	/* 점과 점 사이의 거리, 선과 점 사이의 거리 중 긴 거리
	Point에 선의 시작 위치를 저장하세요.  
	Rotation에 선의 방향을 저장하세요. */
	PointToPoint_PointToLine_Greater,
};

enum class ECharacterManagerQueryScreenDistanceFunction : uint8
{
	/* 점과 점 사이의 거리
	Point에 점의 위치를 저장하세요. */
	PointToPoint,

	/* 선과 점의 내적입니다.
	선의 방향 기준으로 점의 반대편에 있을 경우에는 음수 거리가 발생합니다.
	Point에 선의 시작 위치를 저장하세요.  
	Direction에 선의 방향을 저장하세요. */
	Dot,

	/* 선과 점의 내적입니다.
	음수 거리는 양수 거리로 변환됩니다.
	Point에 선의 시작 위치를 저장하세요.  
	Direction에 선의 방향을 저장하세요. */
	DotAbs,
};

struct FCharacterManagerQueryOption
{
	ECharacterManagerQueryType Type = ECharacterManagerQueryType::Enemy;
};

struct FCharacterManager3DQuery
{
	ECharacterManagerQueryDistancePriority DistancePriority = ECharacterManagerQueryDistancePriority::Near;
	ECharacterManagerQueryDistanceFunction DistanceFunction = ECharacterManagerQueryDistanceFunction::PointToPoint_PointToLine_Smaller;
	FVector Point;
	FRotator Rotation;
};

struct FCharacterManagerScreenQuery
{
	// 활성화하면 음수 거리를 가지는 결과는 무시합니다.
	bool IgnoreNegativeDistance = true;

	ECharacterManagerQueryDistancePriority DistancePriority = ECharacterManagerQueryDistancePriority::Near;
	ECharacterManagerQueryScreenDistanceFunction DistanceFunction = ECharacterManagerQueryScreenDistanceFunction::Dot;
	FVector2D Point = FVector2D::ZeroVector;
	FVector2D LineDirection; // 정규화되지 않은 값이어도 괜찮습니다.
};

struct FCharacterManagerQueryFiler
{
	virtual ~FCharacterManagerQueryFiler() {}
	virtual bool Search(const AActor* character) const = 0;
	virtual bool IsValid(FString& invalidDescription) const { return true; }
};

struct FCharacterManagerQueryDefaultFiler : public FCharacterManagerQueryFiler
{
	virtual bool Search(const AActor* character) const override;
	bool IsNotTargetToIgnore(const AActor* character) const;
	bool TagMatch(const AActor* character) const;

	// 이 캐릭터들은 검색에서 제외됩니다.
	TArray<const AActor*> Ignores; 

	// 태그가 없다면 태그 검사를 하지 않습니다.
	TArray<FName> Tags;			
};

struct FCharacterManagerQueryRaycastFiler : public FCharacterManagerQueryFiler
{
	virtual bool Search(const AActor* character) const override;
	virtual bool IsValid(FString& invalidDescription) const override;
	bool IsNotBlocked(const AActor* character) const;

	// 레이캐스트를 시행하기 위한 시작점입니다.
	FVector RaystartLocation;

	// 검사에 사용할 월드입니다.
	UWorld* World = nullptr;

	// 검사에 사용할 프리셋입니다.
	FName BlockedCheckPreset = GodfallPresets::SweepCollision;

	// 이 캐릭터들은 레이캐스트에서 제외됩니다.
	TArray<const AActor*> Ignores;
};

struct FCharacterManagerQueryDistanceFiler : public FCharacterManagerQueryFiler
{
	virtual bool Search(const AActor* character) const override;
	bool IsInRadius(const AActor* character) const;

	// 거리를 측정하기 위한 기준 점입니다.
	FVector Point;

	// 거리를 비교하기 위한 반지름입니다.
	float Radius;

	// 캡슐의 반지름을 사용하여 캡슐의 모서리부터 기준점까지의 거리를 구합니다.
	bool UseCapsuleRadius = false;
};

struct FCharacterManagerQueryExecutableFiler : public FCharacterManagerQueryFiler
{
	virtual bool Search(const AActor* character) const override;
};

enum class ECharacterManagerQueryResult : uint8
{
	Succeeded,
	Failed,
	InvalidFilter,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GODFALL_API UCharacterManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterManagerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	

public:
	void RegistCharacter(AGodfallCharacterBase* character);
	void UnregistCharacter(AGodfallCharacterBase* character);
	bool Contains(const AGodfallCharacterBase* character) const;

	TArray<TWeakObjectPtr<AActor>> GetEnemies() const { return mEnemies; }

	AGodfallCharacterBase* Query(
		const TArray<const FCharacterManagerQueryFiler*>& f3Dfilters, const FCharacterManager3DQuery& f3DQuery, const FCharacterManagerQueryOption& option,
		const FCharacterManagerScreenQuery* fScreenQuery,
		ECharacterManagerQueryResult* result = nullptr, FString* invalidFilterDescription = nullptr) const;

private:
	void WriteQueryResult(ECharacterManagerQueryResult* outResult, ECharacterManagerQueryResult inResult) const;
	void WriteInvalidFilterDescription(ECharacterManagerQueryResult* result, FString* outDescription, const FString& inDescription) const;
	float CalcDistance(const AActor* character, const FCharacterManager3DQuery& f3DQuery) const;

	// 화면 내부에 없으면 false를 반환합니다.
	bool CalcScreenDistance(const AActor* character, const FCharacterManagerScreenQuery* fScreenQuery, float& distance) const;

private:
	UPROPERTY(VisibleAnywhere, DisplayName = "Characters", Meta = (AllowPrivateAccess))
	TArray<TWeakObjectPtr<AActor>> mCharacters;

	UPROPERTY(VisibleAnywhere, DisplayName = "Enemies", Meta = (AllowPrivateAccess))
	TArray<TWeakObjectPtr<AActor>> mEnemies;
};
