// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstanceSilvermane.h"
#include "GodfallUtil.h"

#include "GodfallEnemyAnimInstanceBase.h"
#include "CGrievesMinion.h"
#include "CSolaris.h"

UAnimInstanceSilvermane::UAnimInstanceSilvermane()
{
	ConstructorHelpers::FObjectFinder<UAnimMontage> damageMontage(TEXT("/Game/Characters/Silvermane/Montages/AM_Silvermane_Damage.AM_Silvermane_Damage"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> additiveFlinchMontage(TEXT("/Game/Characters/Silvermane/Montages/AM_Silvermane_Additive_Flinch.AM_Silvermane_Additive_Flinch"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> deathMontage(TEXT("/Game/Characters/Silvermane/Montages/AM_SIlvermane_Death.AM_SIlvermane_Death"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> stunMontage(TEXT("/Game/Characters/Silvermane/Montages/AM_Silvermane_Stun.AM_Silvermane_Stun"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> equipOnMontage(TEXT("/Game/Characters/Silvermane/Montages/AM_Silvermane_EquipOn.AM_Silvermane_EquipOn"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> equipOffMontage(TEXT("/Game/Characters/Silvermane/Montages/AM_Silvermane_EquipOff.AM_Silvermane_EquipOff"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> blockMontage(TEXT("/Game/Characters/Silvermane/Montages/AM_Silvermane_Block.AM_Silvermane_Block"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> executionMontage(TEXT("/Game/Characters/Silvermane/Montages/AM_Silvermane_Execution.AM_Silvermane_Execution"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> jumpMontage(TEXT("/Game/Characters/Silvermane/Montages/AM_Silvermane_Jump.AM_Silvermane_Jump"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> jumpTravelMontage(TEXT("/Game/Characters/Silvermane/Montages/AM_Silvermane_Jump_Travel.AM_Silvermane_Jump_Travel"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> slideMontage(TEXT("/Game/Characters/Silvermane/Montages/AM_Silvermane_Slide.AM_Silvermane_Slide"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> attackSword1HMontage(TEXT("/Game/Characters/Silvermane/Montages/AM_Silvermane_Attack_Sword1H.AM_Silvermane_Attack_Sword1H"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> attackSword2HMontage(TEXT("/Game/Characters/Silvermane/Montages/AM_Silvermane_Attack_Sword2H.AM_Silvermane_Attack_Sword2H"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> attackPolearmMontage(TEXT("/Game/Characters/Silvermane/Montages/AM_Silvermane_Attack_Polearm.AM_Silvermane_Attack_Polearm"));
	ConstructorHelpers::FObjectFinder<UAnimMontage> attackDualBladesMontage(TEXT("/Game/Characters/Silvermane/Montages/AM_Silvermane_Attack_DualBlades.AM_Silvermane_Attack_DualBlades"));

	if (ensure(damageMontage.Succeeded()))
	{
		mDamageMontage = damageMontage.Object;

		mDamageMontageSection.Stagger_Left_Names = { TEXT("Stagger_Left_1"), TEXT("Stagger_Left_2") };
		mDamageMontageSection.Stagger_Right_Names = { TEXT("Stagger_Right_1"), TEXT("Stagger_Right_2") };
		mDamageMontageSection.Knockback_Names = { TEXT("Knockback_Clash"), TEXT("Knockback_Land") };
	}
	if (ensure(additiveFlinchMontage.Succeeded()))
	{
		mAdditiveFlinchMontage = additiveFlinchMontage.Object;
	}
	if (ensure(deathMontage.Succeeded()))
	{
		mDeathMontage = deathMontage.Object;
	}
	if (ensure(stunMontage.Succeeded()))
	{
		mStunMontage = stunMontage.Object;
	}
	if (ensure(equipOnMontage.Succeeded()))
	{
		mEquipOnMontage = equipOnMontage.Object;
	}
	if (ensure(equipOffMontage.Succeeded()))
	{
		mEquipOffMontage = equipOffMontage.Object;
	}
	if (ensure(blockMontage.Succeeded()))
	{
		mBlockMontage = blockMontage.Object;
	}
	if (ensure(executionMontage.Succeeded()))
	{
		mExecutionMontage = executionMontage.Object;

		mExecutionSections.Add(ACGrievesMinion::StaticClass(), TEXT("GrievesMinion"));
		mExecutionSections.Add(ACSolaris::StaticClass(), TEXT("Solaris"));
	}
	if (ensure(jumpMontage.Succeeded()))
	{
		mJumpMontage = jumpMontage.Object;
	}
	if (ensure(jumpTravelMontage.Succeeded()))
	{
		mJumpTravelMontage = jumpTravelMontage.Object;
	}
	if (ensure(slideMontage.Succeeded()))
	{
		mSlideMontage = slideMontage.Object;
	}
	if (ensure(attackSword1HMontage.Succeeded()))
	{
		mAttackSword1HMontage = attackSword1HMontage.Object;

		AddManualMontageSectionTransition(mAttackSword1HMontage, TEXT("SprintLA"), TEXT("LA1"), 0.4f);
		AddManualMontageSectionTransition(mAttackSword1HMontage, TEXT("LA1"), TEXT("LA2"));
		AddManualMontageSectionTransition(mAttackSword1HMontage, TEXT("LA2"), TEXT("LA3"));
		AddManualMontageSectionTransition(mAttackSword1HMontage, TEXT("LA3"), TEXT("LA4"));
		AddManualMontageSectionTransition(mAttackSword1HMontage, TEXT("LA4"), TEXT("LA1_Repeat"), 0.4f);
		AddManualMontageSectionTransition(mAttackSword1HMontage, TEXT("LA1_Repeat"), TEXT("LA2"));

		AddManualMontageSectionTransition(mAttackSword1HMontage, TEXT("SprintHA"), TEXT("HA2"), 0.4f);
		AddManualMontageSectionTransition(mAttackSword1HMontage, TEXT("HA1"), TEXT("HA2"), 0.4f);
		AddManualMontageSectionTransition(mAttackSword1HMontage, TEXT("HA2"), TEXT("HA3"), 0.4f);
		AddManualMontageSectionTransition(mAttackSword1HMontage, TEXT("HA3"), TEXT("HA4"), 0.4f);
		AddManualMontageSectionTransition(mAttackSword1HMontage, TEXT("HA4"), TEXT("HA1"), 0.4f);
	}
	if (ensure(attackSword2HMontage.Succeeded()))
	{
		mAttackSword2HMontage = attackSword2HMontage.Object;

		AddManualMontageSectionTransition(mAttackSword2HMontage, TEXT("SprintLA"), TEXT("LA1"), 0.4f);
		AddManualMontageSectionTransition(mAttackSword2HMontage, TEXT("LA1"), TEXT("LA2"), 0.4f);
		AddManualMontageSectionTransition(mAttackSword2HMontage, TEXT("LA2"), TEXT("LA3"), 0.4f);
		AddManualMontageSectionTransition(mAttackSword2HMontage, TEXT("LA3"), TEXT("LA1"), 0.4f);

		AddManualMontageSectionTransition(mAttackSword2HMontage, TEXT("SprintHA"), TEXT("HA2"), 0.4f);
		AddManualMontageSectionTransition(mAttackSword2HMontage, TEXT("HA1"), TEXT("HA2"), 0.4f);
		AddManualMontageSectionTransition(mAttackSword2HMontage, TEXT("HA2"), TEXT("HA2"), 0.4f);
	}
	if (ensure(attackPolearmMontage.Succeeded()))
	{
		mAttackPolearmMontage = attackPolearmMontage.Object;

		AddManualMontageSectionTransition(mAttackPolearmMontage, TEXT("SprintLA"), TEXT("LA2"));
		AddManualMontageSectionTransition(mAttackPolearmMontage, TEXT("LA1"), TEXT("LA2"));
		AddManualMontageSectionTransition(mAttackPolearmMontage, TEXT("LA2"), TEXT("LA3"));
		AddManualMontageSectionTransition(mAttackPolearmMontage, TEXT("LA3"), TEXT("LA4"));
		AddManualMontageSectionTransition(mAttackPolearmMontage, TEXT("LA4"), TEXT("LA5"));
		AddManualMontageSectionTransition(mAttackPolearmMontage, TEXT("LA5"), TEXT("LA2"));

		AddManualMontageSectionTransition(mAttackPolearmMontage, TEXT("SprintHA"), TEXT("HA2"));
		AddManualMontageSectionTransition(mAttackPolearmMontage, TEXT("HA1"), TEXT("HA2"));
		AddManualMontageSectionTransition(mAttackPolearmMontage, TEXT("HA2"), TEXT("HA3"));
		AddManualMontageSectionTransition(mAttackPolearmMontage, TEXT("HA3"), TEXT("HA1"));
	}
	if (ensure(attackDualBladesMontage.Succeeded()))
	{
		mAttackDualBladesMontage = attackDualBladesMontage.Object;

		AddManualMontageSectionTransition(mAttackDualBladesMontage, TEXT("SprintLA"), TEXT("LA2"));
		AddManualMontageSectionTransition(mAttackDualBladesMontage, TEXT("LA1"), TEXT("LA2"));
		AddManualMontageSectionTransition(mAttackDualBladesMontage, TEXT("LA2"), TEXT("LA3"));
		AddManualMontageSectionTransition(mAttackDualBladesMontage, TEXT("LA3"), TEXT("LA4"));
		AddManualMontageSectionTransition(mAttackDualBladesMontage, TEXT("LA4"), TEXT("LA1"), 0.4f);

		AddManualMontageSectionTransition(mAttackDualBladesMontage, TEXT("SprintHA"), TEXT("HA1"));
		AddManualMontageSectionTransition(mAttackDualBladesMontage, TEXT("HA1"), TEXT("HA1"));
	}
}

void UAnimInstanceSilvermane::NativeThreadSafeUpdateAnimation(float deltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(deltaTime);

	mDirectShield = mGuardState && Montage_IsPlaying(mBlockMontage);

	SolveEquip();
	SolveExitable();
	SolveAttack();
}

void UAnimInstanceSilvermane::SetMoveDirection(const FVector2D& direction)
{
	FVector2D newDirection = GodfallUtil::Vector2D::EndPointInRect(1.0f, 1.0f, direction);
	
	if (newDirection.SquaredLength() > 0.0f)
	{
		mMoveDirection = newDirection;
	}
}

bool UAnimInstanceSilvermane::PlaySlideMontage(E4Direction direction)
{
	if (!ensure(mSlideMontage))
	{
		return false;
	}

	if (Montage_IsActive(nullptr))
	{
		bool playingEquipMontage = IsPlayingEquipOnMontage() || IsPlayingEquipOffMontage();
		if (playingEquipMontage)
		{

		}
		else if (!IsExitable())
		{
			return false;
		}
	}

	FName section;
	switch (direction)
	{
		case E4Direction::Backward:
			section = TEXT("Slide_Bwd");
			break;
		case E4Direction::Forward:
			section = TEXT("Slide_Fwd");
			break;
		case E4Direction::Left:
			section = TEXT("Slide_Left");
			break;
		case E4Direction::Right:
			section = TEXT("Slide_Right");
			break;
	}

	StopAllMontages(0.0f);
	float result = Montage_PlayAtSection(mSlideMontage, section);
	if (ensureMsgf(result > 0.0f, TEXT("%s is not valid section name."), *section.ToString()))
	{
		return false;
	}

	return true;
}

bool UAnimInstanceSilvermane::PlaySlideMontageAsDirection(FVector2D direction)
{
	E4Direction directionEnum = E4Direction::Forward;
	float absX = FMath::Abs(direction.X);
	float absY = FMath::Abs(direction.Y);
	if (absY >= absX)
	{
		float signY = FMath::Sign(direction.Y);
		if (signY >= 0) directionEnum = E4Direction::Forward;
		else directionEnum = E4Direction::Backward;
	}
	else
	{
		float signX = FMath::Sign(direction.X);
		if (signX >= 0) directionEnum = E4Direction::Right;
		else directionEnum = E4Direction::Left;
	}

	return PlaySlideMontage(directionEnum);
}

bool UAnimInstanceSilvermane::PlaySlideMontageByCurrentMoveDirection()
{
	return PlaySlideMontageAsDirection(mMoveDirection);
}

void UAnimInstanceSilvermane::PlayEquipMontage(ESilvermaneEquipTrigger value)
{
	FName section;
	switch (mWeapon)
	{
		case ESilvermaneWeapon::Sword_1H:
			section = TEXT("1HSword");
			break;
		case ESilvermaneWeapon::Polearm_2H:
			section = TEXT("Polearm");
			break;
		case ESilvermaneWeapon::Sword_2H:
			section = TEXT("2HSword");
			break;
		case ESilvermaneWeapon::Dual_Blades:
			section = TEXT("DualBlades");
			break;
	}

	switch (value)
	{
		case ESilvermaneEquipTrigger::Equip_On:
		{
			//if (IsPlayingEquipOnMontage())
			//{
			//	break;
			//}

			float timeFraction = Montage_GetTimeFraction(mEquipOnMontage, section);
			if (timeFraction > 0.0f && timeFraction < 1.0f) return;

			Montage_Stop(0.0f, mEquipOffMontage);
			Montage_Stop(0.0f, mEquipOnMontage);
			Montage_Play(mEquipOnMontage);
			Montage_JumpToSection(section, mEquipOnMontage);
		}
		break;
		case ESilvermaneEquipTrigger::Equip_Off:
		{
			//if (IsPlayingEquipOffMontage())
			//{
			//	break;
			//}

			float timeFraction = Montage_GetTimeFraction(mEquipOffMontage, section);
			
			if (timeFraction > 0.0f && timeFraction < 1.0f) return;

			Montage_Stop(0.0f, mEquipOnMontage);
			Montage_Stop(0.0f, mEquipOffMontage);
			Montage_Play(mEquipOffMontage);
			Montage_JumpToSection(section, mEquipOffMontage);
		}
		break;
	}
}

void UAnimInstanceSilvermane::StopEquipOnMontage(float blendingOutTime)
{
	Montage_Stop(blendingOutTime, mEquipOnMontage);
}

void UAnimInstanceSilvermane::StopEquipOffMontage(float blendingOutTime)
{
	Montage_Stop(blendingOutTime, mEquipOffMontage);
}

void UAnimInstanceSilvermane::StopEquipMontage(float blendingOutTime)
{
	Montage_Stop(blendingOutTime, mEquipOnMontage);
	Montage_Stop(blendingOutTime, mEquipOffMontage);
}

void UAnimInstanceSilvermane::PlayBlockMontage(ESilvermaneBlock value)
{
	FString section;
	switch (value)
	{
		case ESilvermaneBlock::Soft: section = TEXT("Soft"); break;
		case ESilvermaneBlock::Hard: section = TEXT("Hard"); break;
		case ESilvermaneBlock::Skid: section = TEXT("Skid"); break;
		case ESilvermaneBlock::Skid_Strong: section = TEXT("Skid_Strong"); break;
		case ESilvermaneBlock::Ricochet: section = TEXT("Ricochet"); break;
		case ESilvermaneBlock::Break: section = TEXT("Break"); break;
		case ESilvermaneBlock::Break_Perfect: section = TEXT("Break_Perfect"); break;
	}

	//Montage_Stop(0.0f, mAdditiveFlinchMontage);
	//Montage_Stop(0.0f, mDamageMontage);
	//Montage_Stop(0.0f, mBlockMontage);
	StopAllMontages(0.0f);
	Montage_Play(mBlockMontage);
	Montage_JumpToSection(FName(section), mBlockMontage);
}

bool UAnimInstanceSilvermane::PlayExecutionMontageWithEnemy(AGodfallEnemyBase* enemy)
{
	if (!ensure(enemy)) return false;

	if (Montage_IsActive(mExecutionMontage)) return false;

	if (!enemy->GetGodfallEnemyAnimInstance()->GetExecutionMontage()) return false;
	
	FString* pSectionName = mExecutionSections.Find(enemy->GetClass());
	if (!ensureMsgf(pSectionName, TEXT("There's no exectuion section name of %s Class in section map."), *enemy->GetClass()->GetName())) return false;
	FString sectionName = *pSectionName;
	if (!ensureMsgf(mExecutionMontage->IsValidSectionName(FName(sectionName)), TEXT("There's no exection section name of %s in montage sections."), *sectionName)) return false;

	StopAllMontages(0.0f);
	bool succeeded = Montage_PlayAtSection(mExecutionMontage, FName(sectionName), 0.0f) > 0.0f;
	if (!ensureMsgf(succeeded, TEXT("failed to play %s section in execution montage."), *sectionName)) return false;

	bool canExitExecutionMontage = (enemy->GetHealth() - enemy->GetExecutionDamage() > 0);
	enemy->GetGodfallEnemyAnimInstance()->PlayExecutionMontageSelf(canExitExecutionMontage);

	return true;
}

void UAnimInstanceSilvermane::PlayJumpMontage(ESilvermaneJumpType type)
{
	StopEquipMontage();
	StopAllMontages(0.2f);

	switch (type)
	{
		case ESilvermaneJumpType::Jump:
		{
			Montage_Play(mJumpMontage);
		}
		break;
		case ESilvermaneJumpType::Jump_Travel:
		{
			Montage_Play(mJumpTravelMontage);
		}
		break;
	}

	OnJump.Broadcast();
}

bool UAnimInstanceSilvermane::TrySetAttackType(ESilvermaneAttackType type)
{
	if (Montage_IsPlaying(mExecutionMontage))
	{
		mAttackType = ESilvermaneAttackType::None;
		return true;
	}
	bool playingAttackMontage = IsActiveAttackMontage();
	bool playingEquipMontage = IsActiveEquipOnMontage() || IsActiveEquipOffMontage();
	if (Montage_IsActive(nullptr))
	{
		if (playingAttackMontage)
		{
			if (!CanComboInput())
			{
				return false;
			}
		}
		else if (!IsExitable())
		{
			return false;
		}
	}
	mAttackType = type;
	return true;
}

bool UAnimInstanceSilvermane::IgnoreDamage() const
{
	if (Super::IgnoreDamage()) return true;
	if (Montage_IsPlaying(mExecutionMontage)) return true;
	if (Montage_IsPlaying(mJumpMontage)) return true;
	if (Montage_IsPlaying(mJumpTravelMontage)) return true;
	return false;
}

bool UAnimInstanceSilvermane::DamageToAdditive() const
{
	return false;
}

bool UAnimInstanceSilvermane::IsPlayingBaseStateMachine() const
{
	return GetBlendWeight(TEXT("BaseStateMachine"), TEXT("Default")) > 0.0f;
}

bool UAnimInstanceSilvermane::IsActiveEquipOnMontage() const
{
	return Montage_IsActive(mEquipOnMontage);
}

bool UAnimInstanceSilvermane::IsActiveEquipOffMontage() const
{
	return Montage_IsActive(mEquipOffMontage);
}

bool UAnimInstanceSilvermane::IsPlayingEquipOnMontage() const
{
	return Montage_IsPlaying(mEquipOnMontage);
}

bool UAnimInstanceSilvermane::IsPlayingEquipOffMontage() const
{
	return Montage_IsPlaying(mEquipOffMontage);
}

bool UAnimInstanceSilvermane::IsPlayingSprintAnimation() const
{
	if (GetBlendWeight(TEXT("DefaultStateMachine"), TEXT("Sprint")) > 0.0f) return true;
	return false;
}

float UAnimInstanceSilvermane::GetSprintAnimationBlendWeight() const
{
	return GetBlendWeight(TEXT("DefaultStateMachine"), TEXT("Sprint"));
}

bool UAnimInstanceSilvermane::IsActiveAttackMontage() const
{
	return
		Montage_IsActive(mAttackSword1HMontage) ||
		Montage_IsActive(mAttackSword2HMontage) ||
		Montage_IsActive(mAttackPolearmMontage) ||
		Montage_IsActive(mAttackDualBladesMontage);
}

bool UAnimInstanceSilvermane::IsPlayingAttackMontage() const
{
	return
		Montage_IsPlaying(mAttackSword1HMontage) ||
		Montage_IsPlaying(mAttackSword2HMontage) ||
		Montage_IsPlaying(mAttackPolearmMontage) ||
		Montage_IsPlaying(mAttackDualBladesMontage);
}

bool UAnimInstanceSilvermane::CanRotate() const
{
	if (GetBlendWeight(TEXT("BaseStateMachine"), TEXT("Default")) > 0.0f && 
		GetBlendWeight(TEXT("DefaultStateMachine"), TEXT("Idle")) < 0.5f) return true;
	if (GetBlendWeight(TEXT("BaseStateMachine"), TEXT("ShieldEnable")) > 0.0f) return true;
	if (GetBlendWeight(TEXT("BaseStateMachine"), TEXT("Shield")) > 0.0f) return true;
	return false;
}

bool UAnimInstanceSilvermane::CanNotSetRotator() const
{
	return 
		IsPlayingJumpMontage() ||
		Montage_IsActive(mDamageMontage) ||
		Montage_IsActive(mStunMontage);
}

bool UAnimInstanceSilvermane::CanChangeWeapon() const
{
	if (GetBlendWeight(TEXT("BaseStateMachine"), TEXT("Default")) < 1.0f) return false;
	if (GetBlendWeight(TEXT("EquipStateMachine")) > 0.0f) return false;
	return true;
}

bool UAnimInstanceSilvermane::IsShieldOpen() const
{
	if (Montage_IsPlaying(mBlockMontage)) return true;
	if (Montage_IsPlaying(mExecutionMontage)) return true;

	if (!IsAnyMontagePlaying())
	{
		if (GetBlendWeight(TEXT("BaseStateMachine"), TEXT("ShieldDisable")) > 0.0f) return false;

		if (GetBlendWeight(TEXT("BaseStateMachine"), TEXT("ShieldEnable")) > 0.0f) return true;
		if (GetBlendWeight(TEXT("BaseStateMachine"), TEXT("Shield")) > 0.0f) return true;
	}
	return false;
}

bool UAnimInstanceSilvermane::IsActiveExecutionMontage() const
{
	return Montage_IsActive(mExecutionMontage);
}

bool UAnimInstanceSilvermane::IsPlayingJumpMontage() const
{
	if (Montage_IsPlaying(mJumpMontage)) return true;
	if (Montage_IsPlaying(mJumpTravelMontage)) return true;
	return false;
}

bool UAnimInstanceSilvermane::IsPlayingJumpingSectionOfJumpMontage() const
{
	if (Montage_IsPlayingSection(mJumpMontage, TEXT("Jumping"))) return true;
	if (Montage_IsPlayingSection(mJumpTravelMontage, TEXT("Jumping"))) return true;
	return false;
}

float UAnimInstanceSilvermane::GetJumpingSectionTimeFractionOfJumpMontage() const
{
	if (Montage_IsPlaying(mJumpMontage))
	{
		return Montage_GetTimeFraction(mJumpMontage, TEXT("Jumping"));
	}
	else if (Montage_IsPlaying(mJumpTravelMontage))
	{
		return Montage_GetTimeFraction(mJumpTravelMontage, TEXT("Jumping"));
	}

	return -1.0f;
}

ESilvermaneBlock UAnimInstanceSilvermane::GetCurrentBlockType() const
{
	if (!Montage_IsPlaying(mBlockMontage)) return ESilvermaneBlock::None;

	ESilvermaneBlock currentBlockType = ESilvermaneBlock::None;
	FString currentSection = Montage_GetCurrentSection(mBlockMontage).ToString();
	if (currentSection == TEXT("Soft")) currentBlockType = ESilvermaneBlock::Soft;
	else if (currentSection == TEXT("Hard")) currentBlockType = ESilvermaneBlock::Hard;
	else if (currentSection == TEXT("Skid")) currentBlockType = ESilvermaneBlock::Skid;
	else if (currentSection == TEXT("Skid_Strong")) currentBlockType = ESilvermaneBlock::Skid_Strong;
	else if (currentSection == TEXT("Break")) currentBlockType = ESilvermaneBlock::Break;

	return currentBlockType;
}

void UAnimInstanceSilvermane::SolveEquip()
{
	float sumWeights = 0.0f
		+ GetBlendWeight(TEXT("BaseStateMachine"), TEXT("ShieldEnable"))
		+ GetBlendWeight(TEXT("BaseStateMachine"), TEXT("Shield"));

	if (sumWeights > 0.0f)
	{
		StopEquipMontage(0.0f);
	}
}

void UAnimInstanceSilvermane::SolveExitable()
{
	// 애니메이션 조기중단 구간 노티파이 스테이트를 지나고 있으며,
	// 몽타주가 재생중이고, 이동 입력을 받았다면
	if (IsExitable() && Montage_IsActive(nullptr) && mMoveSpeed != ESilvermaneMoveSpeed::Stop)
	{
#pragma region TIME FRACTION CHECK
		UAnimMontage* currentMontage = GetCurrentActiveMontage();
		if (!currentMontage) return;

		FName currentSection = Montage_GetCurrentSection(currentMontage);
		if (currentSection == NAME_None) return;

		float currentTimeFractionInSection = Montage_GetTimeFraction(currentMontage, currentSection);
		if (currentTimeFractionInSection == 0.0f || currentTimeFractionInSection == 1.0f) return;
		if (currentTimeFractionInSection < 0.01f) return;
#pragma endregion
		mAttackType = ESilvermaneAttackType::None;
		// 모든 몽타주를 중단합니다.
		StopAllMontages(0.4f);
		return;
	}
}

void UAnimInstanceSilvermane::SolveAttack()
{
	if (Montage_IsPlaying(mExecutionMontage))
	{
		mAttackType = ESilvermaneAttackType::None;
		return;
	}

	UAnimMontage* attackMontage = nullptr;
	switch (mWeapon)
	{
		case ESilvermaneWeapon::Sword_1H:
			attackMontage = mAttackSword1HMontage;
			break;
		case ESilvermaneWeapon::Sword_2H:
			attackMontage = mAttackSword2HMontage;
			break;
		case ESilvermaneWeapon::Polearm_2H:
			attackMontage = mAttackPolearmMontage;
			break;
		case ESilvermaneWeapon::Dual_Blades:
			attackMontage = mAttackDualBladesMontage;
			break;
		default:
			return;
	}

	if (mAttackType == ESilvermaneAttackType::None)
	{
		return;
	}

	ESilvermaneAttackType attackType = mAttackType;

	if (!IsActiveAttackMontage())
	{
		FName section;
		switch (attackType)
		{
			case ESilvermaneAttackType::Light:
			{
				if (IsPlayingSprintAnimation())
				{
					section = TEXT("SprintLA");
				}
				else
				{
					section = TEXT("LA1");
				}
			}
			break;
			case ESilvermaneAttackType::Heavy:
			{
				if (IsPlayingSprintAnimation())
				{
					section = TEXT("SprintHA");
				}
				else
				{
					section = TEXT("HA1");
				}
			}
			break;
		}

		Montage_PlayAtSection(attackMontage, section, 0.1f);

		OnAttack.Broadcast();
		mAttackType = ESilvermaneAttackType::None;
	}
	else if(IsTransitable())
	{
		FName currentSection = Montage_GetCurrentSection(attackMontage);
		ESilvermaneAttackType currentAttackType = currentSection.ToString().Contains(TEXT("LA")) ? ESilvermaneAttackType::Light : ESilvermaneAttackType::Heavy;

		switch (currentAttackType)
		{
			case ESilvermaneAttackType::Light:
			{
				switch (attackType)
				{
					case ESilvermaneAttackType::Light:
					{
						Montage_ManualSectionTransit(attackMontage);
					}
					break;
					case ESilvermaneAttackType::Heavy:
					{
						Montage_PlayAtSection(attackMontage, TEXT("HA1"), 0.2f);
					}
					break;
				}
			}
			break;
			case ESilvermaneAttackType::Heavy:
			{
				switch (attackType)
				{
					case ESilvermaneAttackType::Light:
					{
						Montage_PlayAtSection(attackMontage, TEXT("LA1"), 0.2f);
					}
					break;
					case ESilvermaneAttackType::Heavy:
					{
						Montage_ManualSectionTransit(attackMontage);
					}
					break;
				}
			}
			break;
		}

		OnAttack.Broadcast();
		mAttackType = ESilvermaneAttackType::None;
	}
}

void UAnimInstanceSilvermane::AnimNotify_BeginDefault()
{
}

void UAnimInstanceSilvermane::AnimNotify_EndDefault()
{
	AnimNotify_EndSprint();
}

void UAnimInstanceSilvermane::AnimNotify_BeginShield()
{
	OnShield.Broadcast();
}

void UAnimInstanceSilvermane::AnimNotify_EndShield()
{
}

void UAnimInstanceSilvermane::AnimNotify_BeginSprint()
{
	OnBeginSprint.Broadcast();
}

void UAnimInstanceSilvermane::AnimNotify_EndSprint()
{
	OnEndSprint.Broadcast();
}

void UAnimInstanceSilvermane::AnimNotify_EquipOn()
{
	OnEquippedOn.Broadcast();
}

void UAnimInstanceSilvermane::AnimNotify_EquipOff()
{
	OnEquippedOff.Broadcast();
}

void UAnimInstanceSilvermane::AnimNotify_StartJumping()
{
	OnJumpingSection.Broadcast();
}

void UAnimInstanceSilvermane::OnMontageStartedCallback(UAnimMontage* montage)
{
	Super::OnMontageStartedCallback(montage);

	if (montage == mExecutionMontage)
	{
		OnBeginExecution.Broadcast();
	}
}

void UAnimInstanceSilvermane::OnMontageEndedCallback(UAnimMontage* montage, bool interrupted)
{
	Super::OnMontageEndedCallback(montage, interrupted);

	if (montage == mExecutionMontage && !Montage_IsPlaying(mExecutionMontage))
	{
		OnEndExecution.Broadcast(interrupted);
	}
}

void UAnimInstanceSilvermane::AnimNotifyState_CanComboInput(EAnimNotifyStateEvent eEvent, const UGodfallAnimNotifyStateBase* animNotifyState, const FAnimNotifiyStateData& data)
{
	Super::AnimNotifyState_CanComboInput(eEvent, animNotifyState, data);

	if (eEvent == EAnimNotifyStateEvent::Begin)
	{
		mAttackType = ESilvermaneAttackType::None;
	}
}

