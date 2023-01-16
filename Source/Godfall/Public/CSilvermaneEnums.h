#pragma once

#include <EngineMinimal.h>

UENUM(BlueprintType)
enum class ESilvermaneWeapon : uint8
{
	Sword_1H,
	Polearm_2H,
	Sword_2H,
	Dual_Blades,
	Num,
};

UENUM(BlueprintType)
enum class ESilvermaneMoveSpeed : uint8
{
	Stop,
	Walk,
	Jog,
	Sprint,
};

enum class ESilvermaneEquipTrigger : uint8
{
	Equip_On,
	Equip_Off,
};

enum class ESilvermaneBlock : uint8
{
	None,
	Soft,
	Hard,
	Skid,
	Skid_Strong,
	Ricochet,
	Break,
	Break_Perfect,
};

enum class ESilvermaneWeaponSocket : uint8
{
	Hand,
	Back,
};

enum class ESilvermaneJumpType : uint8
{
	Jump,
	Jump_Travel,
};

UENUM(BlueprintType)
enum class ESilvermaneAttackType : uint8
{
	None,
	Light,
	Heavy,
};
