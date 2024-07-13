// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterManagerComponent.h"
#include "GodfallCharacterBase.h"
#include "GodfallEnemyBase.h"

bool FCharacterManagerQueryDefaultFilter::Search(const AActor* character) const
{
	return IsNotTargetToIgnore(character) && TagMatch(character);
}

bool FCharacterManagerQueryDefaultFilter::IsNotTargetToIgnore(const AActor* character) const
{
	if (Ignores.IsEmpty()) return true;

	for (auto& ignore : Ignores)
	{
		if (ignore == character)
		{
			return false;
		}
	}

	return true;
}

bool FCharacterManagerQueryDefaultFilter::TagMatch(const AActor* character) const
{
	if (Tags.IsEmpty()) return true;

	for (auto& searchTag : Tags)
	{
		for (auto& actorTag : character->Tags)
		{
			if (searchTag == actorTag)
			{
				return true;
			}
		}
	}

	return false;
}

bool FCharacterManagerQueryRaycastFilter::Search(const AActor* character) const
{
	return IsNotBlocked(character);
}

bool FCharacterManagerQueryRaycastFilter::IsNotBlocked(const AActor* character) const
{
	FVector start = RaystartLocation;
	FVector end = character->GetActorLocation();

	FCollisionQueryParams params;
	params.AddIgnoredActor(character);
	params.AddIgnoredActors(Ignores);
	
	return !World->LineTraceTestByProfile(start, end, BlockedCheckPreset, params);
}

bool FCharacterManagerQueryRaycastFilter::IsValid(FString& invalidDescription) const
{
	if (!ensure(World))
	{
		invalidDescription = TEXT("World is empty");
		return false;
	}

	bool contains = false;
	TArray<TSharedPtr<FName>> profiles;
	UCollisionProfile::GetProfileNames(profiles);
	for (auto& profileName : profiles)
	{
		if (*profileName == BlockedCheckPreset)
		{
			contains = true;
			break;
		}
	}
	if (!ensure(contains))
	{
		invalidDescription = TEXT("Preset is not contains");
		return false;
	}

	return true;
}

UCharacterManagerComponent::UCharacterManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCharacterManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCharacterManagerComponent::RegistCharacter(AGodfallCharacterBase* character)
{
	if (mCharacters.Contains(character)) return;

	mCharacters.Add(character);

	AGodfallEnemyBase* enemy = Cast<AGodfallEnemyBase>(character);
	if (enemy)
	{
		mEnemies.Add(enemy);
	}
}

void UCharacterManagerComponent::UnregistCharacter(AGodfallCharacterBase* character)
{
	mCharacters.RemoveSingle(character);

	AGodfallEnemyBase* enemy = Cast<AGodfallEnemyBase>(character);
	if (enemy)
	{
		mEnemies.RemoveSingle(enemy);
	}
}

bool UCharacterManagerComponent::Contains(const AGodfallCharacterBase* character) const
{
	return mCharacters.Contains(character);
}

AGodfallCharacterBase* UCharacterManagerComponent::Query(
	const TArray<const FCharacterManagerQueryFilter*>& f3Dfilters, const FCharacterManager3DQuery& f3DQuery, const FCharacterManagerQueryOption& option,
	const FCharacterManagerScreenQuery* fScreenQuery, 
	ECharacterManagerQueryResult* result, FString* invalidFilterDescription) const
{
	const TArray<TWeakObjectPtr<AActor>>* characters = nullptr;

	if (f3Dfilters.IsEmpty())
	{
		return nullptr;
	}

	switch (option.Type)
	{
		case ECharacterManagerQueryType::Character:
			characters = &mCharacters;
			break;
		case ECharacterManagerQueryType::Enemy:
			characters = &mEnemies;
			break;
		default:
			WriteQueryResult(result, ECharacterManagerQueryResult::Failed);
			return nullptr;
	}

	struct FCharacterDistance
	{
		AActor* Character;
		float Distance;
	};

	TArray<TWeakObjectPtr<AActor>> filteredCharacters = *characters;

	if (/*f3DQuery */ 1)
	{
		TArray<FCharacterDistance> f3DDistances;

		// 모든 캐릭터를 순회합니다.
		for (auto& character : filteredCharacters)
		{
			if (!character.IsValid()) continue;
			bool succeeded = true;
			// 각 캐릭터마다 필터 검사를 합니다.
			for (auto& f3Dfilter : f3Dfilters)
			{
				// 예외가 발생하면 쿼리를 중단하고, 실패 사유를 저장합니다.
				if (!ensure(f3Dfilter))
				{
					WriteInvalidFilterDescription(result, invalidFilterDescription, TEXT("Filter is nullptr"));
					return nullptr;
				}
				// 올바르지 않은 필터이면 예외를 발생시키고, 필터에서 발생한 실패 사유를 저장합니다.
				FString invalidDescription;
				if (!ensure(f3Dfilter->IsValid(invalidDescription)))
				{
					WriteInvalidFilterDescription(result, invalidFilterDescription, invalidDescription);
					return nullptr;
				}
				// 캐릭터를 필터에 적용시켜 통과하는지 검사합니다.
				if (!f3Dfilter->Search(character.Get()))
				{
					succeeded = false;
					break;
				}
			}
			// 캐릭터가 모든 필터를 통과했으면, f3DQuery에 설정된 방법대로 거리를 계산합니다.
			// 캐릭터와 거리를 쌍으로 묶어 리스트에 저장합니다.
			if (succeeded)
			{
				float distance = CalcDistance(character.Get(), f3DQuery);
				f3DDistances.Add({ character.Get(), distance });
				if (f3DQuery.DistancePriority == ECharacterManagerQueryDistancePriority::None)
				{
					break;
				}
			}
		}

		switch (f3DQuery.DistancePriority)
		{
			case ECharacterManagerQueryDistancePriority::Near:
				f3DDistances.Sort([](const FCharacterDistance& lhs, const FCharacterDistance& rhs)
				{
					return lhs.Distance < rhs.Distance;
				});
				break;
			case ECharacterManagerQueryDistancePriority::Far:
				f3DDistances.Sort([](const FCharacterDistance& lhs, const FCharacterDistance& rhs)
				{
					return lhs.Distance > rhs.Distance;
				});
				break;
		}

		filteredCharacters.Empty();
		for (auto& distance : f3DDistances)
		{
			filteredCharacters.Add(distance.Character);
		}
	}

	if (fScreenQuery)
	{
		TArray<FCharacterDistance> fScreenDistances;

		for (auto& character : filteredCharacters)
		{
			if (!character.IsValid()) continue;

			float screenDistance;
			if (CalcScreenDistance(character.Get(), fScreenQuery, screenDistance))
			{
				if (fScreenQuery->IgnoreNegativeDistance && screenDistance < 0.0f)
				{
					continue;
				}

				fScreenDistances.Add({ character.Get(), screenDistance });

				if (fScreenQuery->DistancePriority == ECharacterManagerQueryDistancePriority::None)
				{
					break;
				}
			}
		}

		switch (fScreenQuery->DistancePriority)
		{
			case ECharacterManagerQueryDistancePriority::Near:
				fScreenDistances.Sort([](const FCharacterDistance& lhs, const FCharacterDistance& rhs)
				{
					return lhs.Distance < rhs.Distance;
				});
				break;
			case ECharacterManagerQueryDistancePriority::Far:
				fScreenDistances.Sort([](const FCharacterDistance& lhs, const FCharacterDistance& rhs)
				{
					return lhs.Distance > rhs.Distance;
				});
				break;
		}

		filteredCharacters.Empty();
		for (auto& distance : fScreenDistances)
		{
			filteredCharacters.Add(distance.Character);
		}
	}

	if (filteredCharacters.IsEmpty())
	{
		WriteQueryResult(result, ECharacterManagerQueryResult::Failed);
		return nullptr;
	}
	else
	{
		WriteQueryResult(result, ECharacterManagerQueryResult::Succeeded);
		return Cast<AGodfallCharacterBase>(filteredCharacters[0]);
	}
}

void UCharacterManagerComponent::WriteQueryResult(ECharacterManagerQueryResult* outResult, ECharacterManagerQueryResult inResult) const
{
	if (outResult)
	{
		*outResult = inResult;
	}
}

void UCharacterManagerComponent::WriteInvalidFilterDescription(ECharacterManagerQueryResult* result, FString* outDescription, const FString& inDescription) const
{
	if (result)
	{
		*result = ECharacterManagerQueryResult::InvalidFilter;
	}

	if (outDescription)
	{
		*outDescription = inDescription;
	}
}

float UCharacterManagerComponent::CalcDistance(const AActor* character, const FCharacterManager3DQuery& f3DQuery) const
{
	switch (f3DQuery.DistanceFunction)
	{
		case ECharacterManagerQueryDistanceFunction::PointToPoint:
		{
			return FVector::Distance(character->GetActorLocation(), f3DQuery.Point);
		}
		break;
		case ECharacterManagerQueryDistanceFunction::PointToLine:
		{
			return FMath::PointDistToLine(character->GetActorLocation(), f3DQuery.Rotation.RotateVector(FVector::ForwardVector), f3DQuery.Point);
		}
		break;
		case ECharacterManagerQueryDistanceFunction::PointToPoint_PointToLine_Smaller:
		{
			float p2p = FVector::Distance(character->GetActorLocation(), f3DQuery.Point);
			float p2l = FMath::PointDistToLine(character->GetActorLocation(), f3DQuery.Rotation.RotateVector(FVector::ForwardVector), f3DQuery.Point);
			return FMath::Min(p2p, p2l);
		}
		break;
		case ECharacterManagerQueryDistanceFunction::PointToPoint_PointToLine_Greater:
		{
			float p2p = FVector::Distance(character->GetActorLocation(), f3DQuery.Point);
			float p2l = FMath::PointDistToLine(character->GetActorLocation(), f3DQuery.Rotation.RotateVector(FVector::ForwardVector), f3DQuery.Point);
			return FMath::Max(p2p, p2l);
		}
		break;
	}

	return FLT_MAX;
}

bool UCharacterManagerComponent::CalcScreenDistance(
	const AActor* character, const FCharacterManagerScreenQuery* fScreenQuery, float& distance) const
{
	bool outOfScreen = true;
	FVector2D screenLocation;
	FVector2D screenDirection = fScreenQuery->LineDirection.GetSafeNormal();
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	// 캐릭터의 월드 좌표를 스크린 좌표로 변환합니다.
	if (playerController->ProjectWorldLocationToScreen(character->GetActorLocation(), screenLocation))
	{
		int32 ViewportSizeX, ViewportSizeY;
		playerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
		screenLocation.X /= ViewportSizeX;
		screenLocation.Y /= ViewportSizeY;
		// 캐릭터가 화면 내에 존재하는지 검사합니다.
		if (screenLocation.X >= 0.0f && screenLocation.X <= 1.0f &&
			screenLocation.Y >= 0.0f && screenLocation.Y <= 1.0f)
		{
			// 0~1 사이인 스크린 좌표를 -1~1 사이로 매핑합니다.
			screenLocation = screenLocation * 2.0f - 1.0f;
			// 캐릭터가 화면 내에 존재함을 알립니다.
			outOfScreen = false;
		}
	}
	if (outOfScreen)
	{
		return false;
	}
	switch (fScreenQuery->DistanceFunction)
	{
		// 스크린 위치와 스크린 캐릭터 위치 사이의 거리를 구합니다.
		case ECharacterManagerQueryScreenDistanceFunction::PointToPoint:
		{
			distance = FVector2D::Distance(screenLocation, fScreenQuery->Point);
			return true;
		}
		// 스크린 방향벡터와 스크린 캐릭터 위치와 내적합니다.
		case ECharacterManagerQueryScreenDistanceFunction::Dot:
		{
			distance = FVector2D::DotProduct(screenDirection, screenLocation - fScreenQuery->Point);
			return true;
		}
		// 스크린 방향벡터와 스크린 캐릭터 위치와 내적한 결과의 절대값을 반환합니다.
		case ECharacterManagerQueryScreenDistanceFunction::DotAbs:
		{
			distance = FMath::Abs(FVector2D::DotProduct(screenDirection, screenLocation - fScreenQuery->Point));
			return true;
		}
	}
	return false;
}

bool FCharacterManagerQueryDistanceFilter::Search(const AActor* character) const
{
	return IsInRadius(character);
}

bool FCharacterManagerQueryDistanceFilter::IsInRadius(const AActor* character) const
{
	float capsuleRadius = 0.0f;
	if (UseCapsuleRadius)
	{
		const AGodfallCharacterBase* godfallCharacter = Cast<AGodfallCharacterBase>(character);
		capsuleRadius = godfallCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	}

	float distance = FVector::Distance(character->GetActorLocation(), Point);
	distance = FMath::Clamp(distance - capsuleRadius, 0.0f, FLT_MAX);

	return distance < Radius;
}

bool FCharacterManagerQueryExecutableFilter::Search(const AActor* character) const
{
	const AGodfallEnemyBase* godfallEnemy = Cast<AGodfallEnemyBase>(character);
	if (!ensure(godfallEnemy)) return false;

	bool isStun = godfallEnemy->IsStun();
	bool containsExecuteMontage = godfallEnemy->IsExecutableEnemy();

	return isStun && containsExecuteMontage;
}

bool FCharacterManagerQueryAngleFilter::Search(const AActor* character) const 
{
	FVector PointToOtherCharacter = (character->GetActorLocation() - Point).GetSafeNormal();
	float Radian = FMath::Acos(FVector::DotProduct(PointToOtherCharacter, Direction));
	float Angle = FMath::RadiansToDegrees(Radian);
	return Angle < MaxAngle;
}

bool FCharacterManagerQueryAngleFilter::IsValid(FString& invalidDescription) const 
{
	return Direction != FVector::ZeroVector;
}
