// Fill out your copyright notice in the Description page of Project Settings.


#include "CSilvermane.h"
#include "AnimInstanceSilvermane.h"
#include "AShield.h"
#include "GodfallEnemyBase.h"
#include "CharacterRotateComponent.h"
#include "GodfallGameState.h"
#include "CharacterManagerComponent.h"
#include "JumpArea.h"

ACSilvermane::ACSilvermane()
{
	InitSkeletalMesh(TEXT("/Game/Characters/Silvermane/SK_Silvermane.SK_Silvermane"), FVector(0.0f, 0.0f, -98.187494));
	InitAnimInstance(TEXT("/Game/Characters/Silvermane/SK_Silvermane_Skeleton_AnimBlueprint.SK_Silvermane_Skeleton_AnimBlueprint_C"));
	InitCapsule(95.0f, 60.0f);
	
	InitWeaponSocket();
	InitWeaponMesh();
	InitWeaponHitbox();
	SetWeaponVisibility(ESilvermaneWeapon::Sword_1H);
	SetWeaponHitbox(ESilvermaneWeapon::Sword_1H);

	mMaxHealth = 20.0f;
	mMaxStemina = 20.0f;
	mBlockSteminaDamage = 0.0f;

	USkeletalMeshComponent* characterMesh = GetMesh();
	if (ensure(characterMesh))
	{
		characterMesh->SetTeleportDistanceThreshold(100.0f);
		characterMesh->SetTeleportRotationThreshold(45.0f);
		characterMesh->bLocalSpaceSimulation = true;
		characterMesh->ClothBlendWeight = 0.6f;
	}
}

void ACSilvermane::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	USkeletalMeshComponent* characterMesh = GetMesh();
	if (ensure(characterMesh))
	{
		UAnimInstance* animInstance = characterMesh->GetAnimInstance();
		mAnimInstance = Cast<UAnimInstanceSilvermane>(animInstance);
	}
}

void ACSilvermane::BeginPlay()
{
	Super::BeginPlay();

	if (mAnimInstance)
	{
		mAnimInstance->OnBeginSprint.AddUObject(this, &ACSilvermane::OnBeginSprint);
		mAnimInstance->OnEndSprint.AddUObject(this, &ACSilvermane::OnEndSprint);
		mAnimInstance->OnEquippedOff.AddUObject(this, &ACSilvermane::OnEquippedOff);
		mAnimInstance->OnEquippedOn.AddUObject(this, &ACSilvermane::OnEquippedOn);
		mAnimInstance->OnAttack.AddUObject(this, &ACSilvermane::OnAttack);
		mAnimInstance->OnShield.AddUObject(this, &ACSilvermane::OnShield);
		mAnimInstance->OnBeginExecution.AddUObject(this, &ACSilvermane::OnBeginExecution);
		mAnimInstance->OnEndExecution.AddUObject(this, &ACSilvermane::OnEndExecution);
		mAnimInstance->OnJump.AddUObject(this, &ACSilvermane::OnJump);
		mAnimInstance->OnJumpingSection.AddUObject(this, &ACSilvermane::OnJumpingSection);

		mAnimInstance->OnCustomAnimNotifyState.AddDynamic(this, &ACSilvermane::OnCustomAnimNotifyState);
	}

	if (ensure(WeaponLSocket))
	{
		mShield = GetWorld()->SpawnActor<AAShield>(AAShield::StaticClass());
		if (ensure(mShield))
		{
			mShield->AttachToComponent(WeaponLSocket, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}
}

void ACSilvermane::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (mAnimInstance)
	{
		mAnimInstance->SetMoveDirection(mMoveDirection);
		mAnimInstance->SetMoveSpeed(mMoveDirection.Length() > 0.0f ? mMoveSpeed : ESilvermaneMoveSpeed::Stop);
		mAnimInstance->SetGuard(mGuard);

		TickForShield();
		TickForJump();
		TickEquipCheck();
		TickForExecuteLocation();
	}
}

bool ACSilvermane::CanRotate() const
{
	if (!Super::CanRotate()) return false;
	return mAnimInstance->CanRotate();
}

bool ACSilvermane::IsInvincible() const
{
	return mAnimInstance->IsActiveExecutionMontage();
}

EBlockOutput ACSilvermane::CanBlock(const FDamageInput& damageInput) const
{
	if (!mShield->IsOpen())
	{
		float currentTime = GetWorld()->GetTime().GetWorldTimeSeconds();
		if (FMath::Abs(currentTime - mLastGuardTime) > mPerfectBlockLimit)
		{
			return EBlockOutput::Fail;
		}
	}

	if (mAnimInstance->IsPlayingStunMontage())
	{
		return EBlockOutput::Fail;
	}

	FVector thisToCauser = (damageInput.CauserActor->GetActorLocation() - GetActorLocation());
	FVector thisToCauserXY = FVector::VectorPlaneProject(thisToCauser, FVector::UpVector).GetSafeNormal();
	FVector forwardXY = FVector::VectorPlaneProject(GetActorForwardVector(), FVector::UpVector).GetSafeNormal();
	float dot = FVector::DotProduct(thisToCauserXY, forwardXY);
	
	static const float limitAngle = 90.0f;
	static const float limitCos = FMath::Cos(FMath::DegreesToRadians(limitAngle));

	if (dot > limitCos)
	{
		float delta = GetWorld()->GetTime().GetWorldTimeSeconds() - mLastGuardTime;
		
		if (delta < mPerfectBlockLimit)
		{
			return EBlockOutput::Blocked_Perfect;
		}
		else
		{
			return EBlockOutput::Blocked;
		}
	}
	else
	{
		return EBlockOutput::Fail;
	}
}

void ACSilvermane::OnGuard(const FDamageOutput& damageOutput)
{
	switch (damageOutput.DamageOutput.Result)
	{
		case EDamageOutput::Blocked:
		{
			switch (damageOutput.DamageOutput.Power)
			{
				case EDamagePower::Level_1:
					mAnimInstance->PlayBlockMontage(ESilvermaneBlock::Skid_Strong);
					break;
				case EDamagePower::Level_2:
					mAnimInstance->PlayBlockMontage(ESilvermaneBlock::Skid_Strong);
					break;
				case EDamagePower::Level_3:
					mAnimInstance->PlayBlockMontage(ESilvermaneBlock::Break);
					break;
			}

			SetWeaponSocket(ESilvermaneWeaponSocket::Hand);
		}
		break;
		case EDamageOutput::Blocked_Perfect:
		{
			switch (damageOutput.DamageOutput.Power)
			{
				case EDamagePower::Level_1:
					mAnimInstance->PlayBlockMontage(ESilvermaneBlock::Ricochet);
					break;
				case EDamagePower::Level_2:
					mAnimInstance->PlayBlockMontage(ESilvermaneBlock::Ricochet);
					break;
				case EDamagePower::Level_3:
					mAnimInstance->PlayBlockMontage(ESilvermaneBlock::Break_Perfect);
					break;
			}

			SetWeaponSocket(ESilvermaneWeaponSocket::Hand);
		}
		break;
	}
}

void ACSilvermane::OnDamageImpl(const FDamageOutput& output, bool onZeroHealth, bool onMaxStemina)
{
	SetWeaponSocket(ESilvermaneWeaponSocket::Hand);
}

void ACSilvermane::OnCustomAnimNotifyState(EAnimNotifyStateEvent eEvent, const FAnimNotifiyStateData& data, const FName& notifyStateName, const FCustomNotifyStateVariable& variables)
{
	if (notifyStateName == TEXT("Weapon"))
	{
		switch (eEvent)
		{
			case EAnimNotifyStateEvent::Begin:
			{
				if (!ensure(variables.Booleans.Num() > 0)) break;

				if (variables.Booleans[0])
					SetWeaponSocket(ESilvermaneWeaponSocket::Hand);
				else 
					SetWeaponSocket(ESilvermaneWeaponSocket::Back);
			}
			break;
		}
	}
	else if (notifyStateName == TEXT("Shield"))
	{
		switch (eEvent)
		{
			case EAnimNotifyStateEvent::Tick:
			{
				if (!ensure(variables.Booleans.Num() > 0)) break;

				mShieldOverride = true;
				mShieldOverrideOpen = variables.Booleans[0];
			}
			break;
			case EAnimNotifyStateEvent::End:
			{
				if (!ensure(variables.Booleans.Num() > 0)) break;

				mShieldOverride = false;
				mShieldOverrideOpen = variables.Booleans[0];
			}
			break;
		}
	}
}

void ACSilvermane::SetMove(FVector2D direction, ESilvermaneMoveSpeed speed)
{
	mMoveDirection = direction;
	mMoveSpeed = speed;
}

void ACSilvermane::Slide(E4Direction direction)
{
	mAnimInstance->PlaySlideMontage(direction);
}

void ACSilvermane::Slide(FVector2D direction)
{
	mAnimInstance->PlaySlideMontageAsDirection(direction);
}

void ACSilvermane::SlideToCurrentDirection()
{
	Slide(mMoveDirection);
}

void ACSilvermane::SetGuard(bool value)
{
	if (!mGuard && value)
	{
		mLastGuardTime = GetWorld()->GetTime().GetWorldTimeSeconds();
	}

	mGuard = value;
}

bool ACSilvermane::SetLightAttack()
{
	return mAnimInstance->TrySetAttackType(ESilvermaneAttackType::Light);
}

bool ACSilvermane::SetHeavyAttack()
{
	return mAnimInstance->TrySetAttackType(ESilvermaneAttackType::Heavy);
}

bool ACSilvermane::TrySetWeaponType(ESilvermaneWeapon value)
{
	if (!mAnimInstance->CanChangeWeapon()) return false;

	bool withAnimation = !mAnimInstance->IsPlayingSprintAnimation();

	SetWeaponType(value, withAnimation);
	return true;
}

ESilvermaneWeapon ACSilvermane::GetWeaponType() const
{
	return mAnimInstance->GetWeaponType();
}

bool ACSilvermane::TryExecute(AGodfallEnemyBase* targetEnemy)
{
	if (!ensure(targetEnemy)) return false;

	if (!mAnimInstance->PlayExecutionMontageWithEnemy(targetEnemy)) return false;



	//MoveIgnoreActorAdd(enemy);
	AGodfallGameState* gameState = Cast<AGodfallGameState>(GetWorld()->GetGameState());
	if (ensure(gameState))
	{
		auto enemies = gameState->GetCharacterManager()->GetEnemies();
		for (auto& enemy : enemies)
		{
			if (!enemy.IsValid()) continue;
			MoveIgnoreActorAdd(enemy.Get());
		}
	}
	targetEnemy->MoveIgnoreActorAdd(this);



	FVector enemyToSilvermane = (GetActorLocation() - targetEnemy->GetActorLocation());
	enemyToSilvermane = FVector::VectorPlaneProject(enemyToSilvermane, FVector::UpVector).GetSafeNormal();

	float silvermaneRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	float enemyRadius = targetEnemy->GetCapsuleComponent()->GetScaledCapsuleRadius();
	FVector executionLocation = targetEnemy->GetActorLocation() + enemyToSilvermane * (mExecuteDistance);
	FRotator silvermaneRotator = FRotationMatrix::MakeFromX(-enemyToSilvermane).Rotator();
	FRotator enemyRotator = FRotationMatrix::MakeFromX(enemyToSilvermane).Rotator();

	// ##
	//this->SetActorLocation(executionLocation);

	targetEnemy->GetCharacterRotateComponent()->SetRotatorDirect(enemyRotator);
	this->GetCharacterRotateComponent()->SetRotatorDirect(silvermaneRotator);



	mExecutioningTarget = targetEnemy;
	OnBeginExecution();



	return true;
}

bool ACSilvermane::TryJump()
{
	if (mAnimInstance->IsPlayingJumpMontage()) return false;

	TArray<FOverlapResult> results;
	FCollisionQueryParams params(GodfallTags::JumpArea, false, this);

	FVector capsuleLocation = GetCapsuleComponent()->GetComponentLocation();
	FQuat capsuleRotation = GetCapsuleComponent()->GetComponentRotation().Quaternion();
	FCollisionShape capsule = GetCapsuleComponent()->GetCollisionShape();

	GetWorld()->OverlapMultiByProfile(
		results,
		capsuleLocation, capsuleRotation,
		GodfallPresets::OverlapAll,
		capsule,
		params);

	for (auto& result : results)
	{
		AJumpArea* jumpArea = Cast<AJumpArea>(result.GetActor());
		if (!jumpArea) continue;

		if (jumpArea->PlayerIsOverlapping())
		{
			FParabolaCoefficient3 coefficient;
			if (!jumpArea->GetParabolaCoefficient(&coefficient)) continue;
			if (!coefficient.IsValid()) continue;

			mParabolaJumpCoefficient = coefficient;

			ESilvermaneJumpType jumpType = ESilvermaneJumpType::Jump;
			switch (jumpArea->GetJumpAreaType())
			{
				case EJumpAreaType::Jump:
					jumpType = ESilvermaneJumpType::Jump;
					break;
				case EJumpAreaType::Jump_Travel:
					jumpType = ESilvermaneJumpType::Jump_Travel;
					break;
			}

			FVector thisToEndPoint = (mParabolaJumpCoefficient.P2 - GetActorLocation()).GetSafeNormal();
			thisToEndPoint = FVector::VectorPlaneProject(thisToEndPoint, FVector::UpVector);
			FRotator rotator = FRotationMatrix::MakeFromX(thisToEndPoint).Rotator();

			GetCharacterRotateComponent()->SetRotator(rotator);
			mAnimInstance->PlayJumpMontage(jumpType);

			return true;
		}
	}

	return false;
}

void ACSilvermane::InitWeaponSocket()
{
	if (ensure(GetMesh()->DoesSocketExist(TEXT("weapon_r_socket"))))
	{
		WeaponRSocketComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon RSocket"));
		WeaponRSocketComponent->SetupAttachment(GetMesh(), TEXT("weapon_r_socket"));
	}

	if (ensure(GetMesh()->DoesSocketExist(TEXT("weapon_l_socket"))))
	{
		WeaponLSocket = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon LSocket"));
		WeaponLSocket->SetupAttachment(GetMesh(), TEXT("weapon_l_socket"));
	}

	if (ensure(GetMesh()->DoesSocketExist(TEXT("spine_03_1h_sword_socket"))))
	{
		Sword1HBackSocket = CreateDefaultSubobject<USceneComponent>(TEXT("Sword1H Back Socket"));
		Sword1HBackSocket->SetupAttachment(GetMesh(), TEXT("spine_03_1h_sword_socket"));

		Sword1HMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Sword1H Mesh"));
		Sword1HMesh->SetupAttachment(WeaponRSocketComponent);
	}

	if (ensure(GetMesh()->DoesSocketExist(TEXT("spine_03_2h_sword_socket"))))
	{
		Sword2HBackSocket = CreateDefaultSubobject<USceneComponent>(TEXT("Sword2H Back Socket"));
		Sword2HBackSocket->SetupAttachment(GetMesh(), TEXT("spine_03_2h_sword_socket"));

		Sword2HMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Sword2H Mesh"));
		Sword2HMesh->SetupAttachment(WeaponRSocketComponent);
	}

	if (ensure(GetMesh()->DoesSocketExist(TEXT("spine_03_polearm_socket"))))
	{
		PolearmBackSocket = CreateDefaultSubobject<USceneComponent>(TEXT("Polearm Back Socket"));
		PolearmBackSocket->SetupAttachment(GetMesh(), TEXT("spine_03_polearm_socket"));

		PolearmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Polearm Mesh"));
		PolearmMesh->SetupAttachment(WeaponRSocketComponent);
	}

	if (ensure(GetMesh()->DoesSocketExist(TEXT("spine_03_dualblades_l_socket"))))
	{
		DualBladesLeftBackSocket = CreateDefaultSubobject<USceneComponent>(TEXT("Dualblades Left Back Socket"));
		DualBladesLeftBackSocket->SetupAttachment(GetMesh(), TEXT("spine_03_dualblades_l_socket"));

		DualbladesLeftMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Dualblades Left Mesh"));
		DualbladesLeftMesh->SetupAttachment(WeaponLSocket);
	}

	if (ensure(GetMesh()->DoesSocketExist(TEXT("spine_03_dualblades_r_socket"))))
	{
		DualBladesRightBackSocket = CreateDefaultSubobject<USceneComponent>(TEXT("Dualblades Right Back Socket"));
		DualBladesRightBackSocket->SetupAttachment(GetMesh(), TEXT("spine_03_dualblades_r_socket"));

		DualbladesRightMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Dualblades Right Mesh"));
		DualbladesRightMesh->SetupAttachment(WeaponRSocketComponent);
	}
}

void ACSilvermane::InitWeaponMesh()
{
	ConstructorHelpers::FObjectFinder<USkeletalMesh> sword1H(TEXT("/Game/Weapons/Sword1H/ArgentBlade/SK_1H_Sword_ArgentBlade.SK_1H_Sword_ArgentBlade"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> sword2H(TEXT("/Game/Weapons/Sword2H/Armageddon_Player/SK_2H_Sword_Armageddon_Player.SK_2H_Sword_Armageddon_Player"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> polearm(TEXT("/Game/Weapons/Polearm/Bloodweep_Player/SK_2H_Polearm_Bloodweep_Player.SK_2H_Polearm_Bloodweep_Player"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> dualblades(TEXT("/Game/Weapons/Dual_Blades/Abstinence/SK_1H_Dual_Abstinence.SK_1H_Dual_Abstinence"));

	if (ensure(sword1H.Succeeded())) Sword1HMesh->SetSkeletalMesh(sword1H.Object);
	if (ensure(sword2H.Succeeded())) Sword2HMesh->SetSkeletalMesh(sword2H.Object);
	if (ensure(polearm.Succeeded())) PolearmMesh->SetSkeletalMesh(polearm.Object);
	if (ensure(dualblades.Succeeded())) DualbladesLeftMesh->SetSkeletalMesh(dualblades.Object);
	if (ensure(dualblades.Succeeded())) DualbladesRightMesh->SetSkeletalMesh(dualblades.Object);
}

void ACSilvermane::InitWeaponHitbox()
{
	Sword1HHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Sword1H Hitbox"));
	Sword2HHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Sword2H Hitbox"));
	PolearmHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Polearm Hitbox"));
	DualbladesLeftHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Dualblades Left Hitbox"));
	DualbladesRightHitbox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Dualblades Right Hitbox"));

	Sword1HHitbox->SetRelativeLocation(FVector(0.0f, 0.0f, -70.0f));
	Sword2HHitbox->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
	PolearmHitbox->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
	DualbladesLeftHitbox->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));
	DualbladesRightHitbox->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));

	Sword1HHitbox->InitCapsuleSize(22.0f, 75.0f);
	Sword2HHitbox->InitCapsuleSize(22.0f, 100.0f);
	PolearmHitbox->InitCapsuleSize(33.0f, 80.0f);
	DualbladesLeftHitbox->InitCapsuleSize(22.0f, 55.0f);
	DualbladesRightHitbox->InitCapsuleSize(22.0f, 55.0f);

	Sword1HHitbox->SetupAttachment(Sword1HMesh);
	Sword2HHitbox->SetupAttachment(Sword2HMesh);
	PolearmHitbox->SetupAttachment(PolearmMesh);
	DualbladesLeftHitbox->SetupAttachment(DualbladesLeftMesh);
	DualbladesRightHitbox->SetupAttachment(DualbladesRightMesh);

	InitHitboxIndex(0, Sword1HHitbox);
	InitHitboxIndex(0, Sword2HHitbox);
	InitHitboxIndex(0, PolearmHitbox);
	InitHitboxIndex(0, DualbladesRightHitbox);
	InitHitboxIndex(1, DualbladesLeftHitbox);
}

void ACSilvermane::SetWeaponSocket(ESilvermaneWeaponSocket value)
{
	//if (GetWeaponSocket() == value) return;

	bool allWeaponMeshExist =
		ensure(Sword1HMesh) &&
		ensure(Sword2HMesh) &&
		ensure(PolearmMesh) &&
		ensure(DualbladesLeftMesh) &&
		ensure(DualbladesRightMesh) &&
		ensure(WeaponLSocket) &&
		ensure(WeaponRSocketComponent) &&
		ensure(Sword1HBackSocket) &&
		ensure(Sword2HBackSocket) &&
		ensure(PolearmBackSocket) &&
		ensure(DualBladesLeftBackSocket) &&
		ensure(DualBladesRightBackSocket);
	if (!allWeaponMeshExist) return;

	const FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);

	switch (value)
	{
		case ESilvermaneWeaponSocket::Hand:
		{
			Sword1HMesh->AttachToComponent(WeaponRSocketComponent, rules);
			Sword2HMesh->AttachToComponent(WeaponRSocketComponent, rules);
			PolearmMesh->AttachToComponent(WeaponRSocketComponent, rules);
			DualbladesLeftMesh->AttachToComponent(WeaponLSocket, rules);
			DualbladesRightMesh->AttachToComponent(WeaponRSocketComponent, rules);
		}
		break;
		case ESilvermaneWeaponSocket::Back:
		{
			Sword1HMesh->AttachToComponent(Sword1HBackSocket, rules);
			Sword2HMesh->AttachToComponent(Sword2HBackSocket, rules);
			PolearmMesh->AttachToComponent(PolearmBackSocket, rules);
			DualbladesLeftMesh->AttachToComponent(DualBladesLeftBackSocket, rules);
			DualbladesRightMesh->AttachToComponent(DualBladesRightBackSocket, rules);
		}
		break;
	}
}

ESilvermaneWeaponSocket ACSilvermane::GetWeaponSocket() const
{
	auto parent = Sword1HMesh->GetAttachParent();

	if (parent == WeaponRSocketComponent) return ESilvermaneWeaponSocket::Hand;
	else return ESilvermaneWeaponSocket::Back;
}


void ACSilvermane::SetWeaponVisibility(ESilvermaneWeapon value)
{
	bool sword1h = false;
	bool sword2h = false;
	bool polearm = false;
	bool dualBlades = false;
	switch (value)
	{
		case ESilvermaneWeapon::Sword_1H: sword1h = true; break;
		case ESilvermaneWeapon::Sword_2H: sword2h = true; break;
		case ESilvermaneWeapon::Polearm_2H: polearm = true; break;
		case ESilvermaneWeapon::Dual_Blades: dualBlades = true; break;
	}
	Sword1HMesh->SetVisibility(sword1h);
	Sword2HMesh->SetVisibility(sword2h);
	PolearmMesh->SetVisibility(polearm);
	DualbladesLeftMesh->SetVisibility(dualBlades);
	DualbladesRightMesh->SetVisibility(dualBlades);
}

void ACSilvermane::SetWeaponHitbox(ESilvermaneWeapon value)
{
	bool sword1h = false;
	bool sword2h = false;
	bool polearm = false;
	bool dualBlades = false;
	switch (value)
	{
		case ESilvermaneWeapon::Sword_1H: sword1h = true; break;
		case ESilvermaneWeapon::Sword_2H: sword2h = true; break;
		case ESilvermaneWeapon::Polearm_2H: polearm = true; break;
		case ESilvermaneWeapon::Dual_Blades: dualBlades = true; break;
	}

	Sword1HHitbox->SetCollisionProfileName(sword1h ? GodfallPresets::HitTrigger : GodfallPresets::Disable);
	Sword2HHitbox->SetCollisionProfileName(sword2h ? GodfallPresets::HitTrigger : GodfallPresets::Disable);
	PolearmHitbox->SetCollisionProfileName(polearm ? GodfallPresets::HitTrigger : GodfallPresets::Disable);
	DualbladesLeftHitbox->SetCollisionProfileName(dualBlades ? GodfallPresets::HitTrigger : GodfallPresets::Disable);
	DualbladesRightHitbox->SetCollisionProfileName(dualBlades ? GodfallPresets::HitTrigger : GodfallPresets::Disable);
}

void ACSilvermane::SetWeaponType(ESilvermaneWeapon value, bool withAnimation)
{
	mAnimInstance->SetWeaponType(value);
	SetWeaponVisibility(value);
	SetWeaponHitbox(value);

	if (withAnimation)
	{
		SetWeaponSocket(ESilvermaneWeaponSocket::Back);
		mAnimInstance->PlayEquipMontage(ESilvermaneEquipTrigger::Equip_On);
	}
	else
	{
		SetWeaponSocket(ESilvermaneWeaponSocket::Back);
		mAnimInstance->StopEquipMontage(0.1f);
	}
}

void ACSilvermane::OnBeginSprint()
{
	if (GetWeaponSocket() == ESilvermaneWeaponSocket::Hand)
	{
		mAnimInstance->PlayEquipMontage(ESilvermaneEquipTrigger::Equip_Off);
	}
	if (mAnimInstance->IsActiveEquipOnMontage())
	{
		mAnimInstance->StopEquipOnMontage(0.1f);
	}
}

void ACSilvermane::OnEndSprint()
{
	if (GetWeaponSocket() == ESilvermaneWeaponSocket::Back)
	{
		mAnimInstance->PlayEquipMontage(ESilvermaneEquipTrigger::Equip_On);
	}
	if (mAnimInstance->IsActiveEquipOffMontage())
	{
		mAnimInstance->StopEquipOffMontage(0.1f);
	}
}

void ACSilvermane::OnEquippedOff()
{
	SetWeaponSocket(ESilvermaneWeaponSocket::Back);
}

void ACSilvermane::OnEquippedOn()
{
	SetWeaponSocket(ESilvermaneWeaponSocket::Hand);
}

void ACSilvermane::OnAttack()
{
	mAnimInstance->StopEquipMontage();
	SetWeaponSocket(ESilvermaneWeaponSocket::Hand);
}

void ACSilvermane::OnShield()
{
	mAnimInstance->StopEquipMontage(0.1f);
	SetWeaponSocket(ESilvermaneWeaponSocket::Hand);
}

void ACSilvermane::OnBeginExecution()
{
	mAnimInstance->StopEquipMontage();

	SetWeaponSocket(ESilvermaneWeaponSocket::Back);
}

void ACSilvermane::OnEndExecution(bool interruped)
{
	//if (interruped)
	//{
	//	SetWeaponSocket(ESilvermaneWeaponSocket::Hand);
	//}
	//else if(GetWeaponSocket() == ESilvermaneWeaponSocket::Back)
	//{
	//	mAnimInstance->PlayEquipMontage(ESilvermaneEquipTrigger::Equip_On);
	//}

	if (mExecutioningTarget)
	{
		//MoveIgnoreActorRemove(mExecutionTarget);
		AGodfallGameState* gameState = Cast<AGodfallGameState>(GetWorld()->GetGameState());
		if (ensure(gameState))
		{
			auto enemies = gameState->GetCharacterManager()->GetEnemies();
			for (auto& enemy : enemies)
			{
				if (!enemy.IsValid()) continue;
				MoveIgnoreActorRemove(enemy.Get());
			}
		}
		mExecutioningTarget->MoveIgnoreActorRemove(this);

		mExecutioningTarget = nullptr;
	}
}

void ACSilvermane::OnJump()
{
	SetWeaponSocket(ESilvermaneWeaponSocket::Hand);
}

void ACSilvermane::OnJumpingSection()
{
	// Feet location
	FVector feet = GetActorLocation() + FVector::DownVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	mParabolaJumpCoefficient.UpdateStartPoint(feet);
}

void ACSilvermane::TickForShield()
{
	if (mShield)
	{
		bool open = false;
		if (mShieldOverride) open = mShieldOverrideOpen;
		else open = mAnimInstance->IsShieldOpen();
		mShield->SetOpen(open);
	}
}

void ACSilvermane::TickEquipCheck()
{
	if (mAnimInstance->IsAnyMontagePlaying() && !(mAnimInstance->IsPlayingEquipOnMontage() || mAnimInstance->IsPlayingEquipOffMontage()))
	{
		return;
	}

	switch (GetWeaponSocket())
	{
		case ESilvermaneWeaponSocket::Hand:
		{
			//if (mAnimInstance->IsAnyMontagePlaying() && (mAnimInstance->IsPlayingEquipOnMontage() || mAnimInstance->IsPlayingEquipOffMontage()))
			//{
			//	return;
			//}

			bool isPlayingBaseStateMachine = mAnimInstance->IsPlayingBaseStateMachine();
			bool isPlayingSprint = mAnimInstance->GetSprintAnimationBlendWeight() >= 1.0f;

			if (isPlayingBaseStateMachine)
			{
				if (isPlayingSprint)
				{
					mAnimInstance->PlayEquipMontage(ESilvermaneEquipTrigger::Equip_Off);
				}
			}
		}
		break;
		case ESilvermaneWeaponSocket::Back:
		{
			//if (mAnimInstance->IsAnyMontagePlaying())
			//{
			//	if (mAnimInstance->IsPlayingEquipOffMontage() && (mAnimInstance->IsPlayingEquipOnMontage() || mAnimInstance->IsPlayingEquipOffMontage()))
			//	{
			//		return;
			//	}
			//}

			bool isPlayingBaseStateMachine = mAnimInstance->IsPlayingBaseStateMachine();
			bool isPlayingSprint = mAnimInstance->GetSprintAnimationBlendWeight() >= 1.0f;

			if (isPlayingBaseStateMachine)
			{
				if (!isPlayingSprint)
				{
					mAnimInstance->PlayEquipMontage(ESilvermaneEquipTrigger::Equip_On);
				}
			}
		}
		break;
	}
}

void ACSilvermane::TickForJump()
{
	if (!mAnimInstance->IsPlayingJumpMontage()) return;
	if (!mAnimInstance->IsPlayingJumpingSectionOfJumpMontage()) return;

	float tiemFractionInSection = mAnimInstance->GetJumpingSectionTimeFractionOfJumpMontage();

	FVector feetLocation = GodfallUtil::Vector3D::Parabola(mParabolaJumpCoefficient, tiemFractionInSection);
	FVector actorLocation = feetLocation + FVector::UpVector *GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	SetActorLocation(actorLocation);
}

void ACSilvermane::TickForExecuteLocation()
{
	if (mExecutioningTarget && mAnimInstance->IsActiveExecutionMontage())
	{
		// player root -> utility_01
		// player root -> enemy root

		FVector silvermaneRoot = GetMesh()->GetBoneLocation(TEXT("root"));
		FVector utility_01 = GetMesh()->GetBoneLocation(TEXT("utility_01"));
		FVector enemyRoot = mExecutioningTarget->GetMesh()->GetBoneLocation(TEXT("root"));

		FVector enemyToSilvermane = silvermaneRoot - utility_01;
		FVector newSilvermaneRootLocation = enemyRoot + enemyToSilvermane;
		FVector newSilvermaneLocation = newSilvermaneRootLocation + FVector::UpVector * GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		newSilvermaneLocation.Z = GetActorLocation().Z;

		SetActorLocation(newSilvermaneLocation);
	}
}
