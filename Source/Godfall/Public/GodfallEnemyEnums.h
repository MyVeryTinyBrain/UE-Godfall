#pragma once

#include <EngineMinimal.h>

UENUM(BlueprintType)
enum class EEnemyMoveSpeed : uint8
{
	None,
	Walk,
	Run,
};

enum class EEnemyAvoid3Direction : uint8
{
	Bwd,
	Left,
	Right,
};

UENUM(BlueprintType)
enum class EEnemyMoveType : uint8
{
	Walk,
	Run,
};
