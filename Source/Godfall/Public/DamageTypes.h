#pragma once

#include "Godfall.h"
#include "DamageTypes.generated.h"

class AActor;
class AGodfallCharacterBase;

struct FDamageOutput;

enum class EBlockOutput : uint8
{
	Fail,
	Blocked,
	Blocked_Perfect,
};

enum class EDamageOutput : uint8
{
	Ignored,
	Blocked,
	Blocked_Perfect,
	Hit,
};

UENUM(BlueprintType)
enum class EDamagePower : uint8
{
	Level_1,
	Level_2,
	Level_3,
};

UENUM(BlueprintType)
enum class EDamageDirection : uint8
{
	Left,
	Right,
};

enum class EDamage : uint8
{
	None,
	Flinch,
	Stagger,
	Knockback,

	Other,
};

struct FDamagePreOutput
{
	AActor* CauserActor = CantBeNull;
	AGodfallCharacterBase* CauserCharacter = Nullable;
	float HealthDamage = 1.0f;
	float SteminaDamage = 1.0f;
	EDamagePower Power = EDamagePower::Level_1;
	EDamageDirection DamageDirection = EDamageDirection::Left;
	UPrimitiveComponent* Hitbox = CantBeNull;
	EDamageOutput Result = EDamageOutput::Hit;
};

struct FDamageInput
{
	AActor* CauserActor = CantBeNull;
	AGodfallCharacterBase* CauserCharacter = Nullable;
	float HealthDamage = 1.0f;
	float SteminaDamage = 1.0f;
	bool UnstoppableDamage = false;
	EDamagePower Power = EDamagePower::Level_1;
	EDamageDirection DamageDirection = EDamageDirection::Left;
	UPrimitiveComponent* Hitbox = nullptr;
};

struct FDamageOutput
{
	AGodfallCharacterBase* DamagedCharacter = CantBeNull;
	FDamagePreOutput DamageOutput;
};

USTRUCT(BlueprintType, Blueprintable)
struct FDamagableData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int HitboxIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDamagePower Power = EDamagePower::Level_2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (EditCondition = "Power != EDamagePower::Level_3", EditConditionHides))
	EDamageDirection DamageDirection = EDamageDirection::Left;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (EditCondition = "Power == EDamagePower::Level_3", EditConditionHides))
	bool UnstoppableDamage = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthDamageMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SteminaDamageMultiplier = 1.0f;
};

struct FDamagableInput
{
	FDamagableData Data;
	UPrimitiveComponent* Hitbox = nullptr;
};
