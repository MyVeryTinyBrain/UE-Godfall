// Fill out your copyright notice in the Description page of Project Settings.


#include "CPlayer.h"
#include "AnimInstanceSilvermane.h"
#include "CharacterRotateComponent.h"
#include "CameraDirector.h"
#include "GodfallGameState.h"
#include "CharacterManagerComponent.h"
#include "GodfallEnemyBase.h"
#include "GodfallPlayerController.h"
#include "PlayerHUD.h"

ACPlayer::ACPlayer()
{
	Tags.Add(GodfallTags::Player);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	AutoPossessAI = EAutoPossessAI::Disabled;
}

void ACPlayer::SetupPlayerInputComponent(UInputComponent* inputComp)
{
	Super::SetupPlayerInputComponent(inputComp);

	inputComp->BindAxis(TEXT("Move Forward / Backward"), this, &ACPlayer::InputMoveForwardBackward);
	inputComp->BindAxis(TEXT("Move Right / Left"), this, &ACPlayer::InputMoveRightLeft);
	inputComp->BindAxis(TEXT("Turn Right / Left Mouse"), this, &ACPlayer::InputTurnRightLeft);
	inputComp->BindAxis(TEXT("Look Up / Down Mouse"), this, &ACPlayer::InputLookUpDown);
	inputComp->BindAxis(TEXT("Walk"), this, &ACPlayer::InputWalk);
	inputComp->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACPlayer::InputSlidePressed);
	inputComp->BindAxis(TEXT("Guard"), this, &ACPlayer::InputGuard);
	inputComp->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &ACPlayer::InputAttackPressed);
	inputComp->BindAction(TEXT("Attack"), EInputEvent::IE_Released, this, &ACPlayer::InputAttackReleased);
	inputComp->BindAxis(TEXT("Sprint"), this, &ACPlayer::InputSprint);
	inputComp->BindAxis(TEXT("Change Weapon"), this, &ACPlayer::InputChangeWeapon);
	inputComp->BindAction(TEXT("Test"), EInputEvent::IE_Pressed, this, &ACPlayer::InputTestPressed);
	inputComp->BindAction(TEXT("Lock On"), EInputEvent::IE_Pressed, this, &ACPlayer::InputLockOnPressed);
}

void ACPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	bUseControllerRotationYaw = false;
}

void ACPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	UpdatePlayerHUD();
}

void ACPlayer::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters params;
	mCameraDirector = GetWorld()->SpawnActor<ACameraDirector>(ACameraDirector::StaticClass(), GetActorLocation(), GetActorRotation());
	mCameraDirector->mTargetActor = this;
	APlayerController* playerController = Cast<APlayerController>(GetController());
	if (!playerController)
	{
		playerController = GetWorld()->GetFirstPlayerController<APlayerController>();
	}
	playerController->SetViewTarget(mCameraDirector);
}

void ACPlayer::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (AGodfallPlayerController* playerController = GetController<AGodfallPlayerController>())
	{
		playerController->ShowHUD = this->ShowHUD;
	}

	if (!mAnimInstance->CanNotSetRotator())
	{
		bool bRotateToMoveDirection = false;
		if (mAnimInstance->IsPlayingSprintAnimation())
			bRotateToMoveDirection = true;
		if (mAnimInstance->IsShieldOpen())
			bRotateToMoveDirection = false;

		if (!bRotateToMoveDirection) RotateToCameraForward();
		else RotateToMoveDirection();
	}

	AccAttackPressed();

	TryChangeLockOnTarget(deltaTime, mMouseDelta);
}

void ACPlayer::OnHealthChanged(float prev, float changed)
{
	UpdatePlayerHUD();
}

void ACPlayer::OnSteminaChanged(float prev, float changed)
{
	UpdatePlayerHUD();
}

void ACPlayer::InputMoveForwardBackward(float input)
{
	mMoveDirection.Y = input;
}

void ACPlayer::InputMoveRightLeft(float input)
{
	mMoveDirection.X = input;
}

void ACPlayer::InputTurnRightLeft(float input)
{
	if (ensure(mCameraDirector))
	{
		mCameraDirector->InputTurnRightLeft(input);
	}

	mMouseDelta.X = input;
}

void ACPlayer::InputLookUpDown(float input)
{
	if (ensure(mCameraDirector))
	{
		mCameraDirector->InputLookUpDown(input);
	}

	mMouseDelta.Y = input;
}

void ACPlayer::InputWalk(float input)
{
	if (input > 0.0f)
	{
		mMoveSpeed = ESilvermaneMoveSpeed::Walk;
	}
	else if(mMoveSpeed != ESilvermaneMoveSpeed::Sprint)
	{
		mMoveSpeed = ESilvermaneMoveSpeed::Jog;
	}
}

void ACPlayer::InputSprint(float input)
{
	if (input > 0.0f)
	{
		mMoveSpeed = ESilvermaneMoveSpeed::Sprint;
	}
	else if (mMoveSpeed != ESilvermaneMoveSpeed::Walk)
	{
		mMoveSpeed = ESilvermaneMoveSpeed::Jog;
	}
}

void ACPlayer::InputSlidePressed()
{
	if (TryJump())
	{
		return;
	}

	SlideToCurrentDirection();
}

void ACPlayer::InputGuard(float input)
{
	SetGuard(input > 0.0f);
}

void ACPlayer::InputAttackPressed()
{
	if (mAnimInstance->IsActiveExecutionMontage())
	{
		return;
	}

	mAttackPressedAcc = 0.0f;
	mAttackPressed = true;
}

void ACPlayer::InputAttackReleased()
{
	mAttackPressed = false;
	if (mAttackPressedAcc < mHeavyAttackAccTime)
	{
		OnLightAttack();
	}
}

void ACPlayer::InputChangeWeapon(float input)
{
	if (input != 0.0f)
	{
		int iWeaponType = (int)GetWeaponType();
		if (input > 0.0f)
		{
			if (++iWeaponType > (int)ESilvermaneWeapon::Num - 1)
			{
				iWeaponType = 0;
			}
		}
		else if (input < 0.0f)
		{
			if (--iWeaponType < 0)
			{
				iWeaponType = (int)ESilvermaneWeapon::Num - 1;
			}
		}

		ESilvermaneWeapon eWeaponType = (ESilvermaneWeapon)iWeaponType;
		TrySetWeaponType(eWeaponType);
	}
}

void ACPlayer::InputTestPressed()
{
}

void ACPlayer::InputLockOnPressed()
{
	ToggleLockOn();
}

bool ACPlayer::OnLightAttack()
{
	if (mAnimInstance->IsActiveExecutionMontage())
	{
		return true;
	}

	AGodfallGameState* gameState = Cast<AGodfallGameState>(GetWorld()->GetGameState());
	if (ensure(gameState))
	{
		TArray<const FCharacterManagerQueryFilter*> f3Dfilters;

		FCharacterManagerQueryExecutableFilter executableFilter;
		f3Dfilters.Add(&executableFilter);

		FCharacterManagerQueryDistanceFilter distanceFilter;
		distanceFilter.Point = GetActorLocation();
		distanceFilter.Radius = mExecuteDistance + GetCapsuleComponent()->GetScaledCapsuleRadius();
		distanceFilter.UseCapsuleRadius = true;
		f3Dfilters.Add(&distanceFilter);

		FCharacterManagerQueryRaycastFilter raycastFilter;
		raycastFilter.RaystartLocation = GetActorLocation();
		raycastFilter.World = GetWorld();
		raycastFilter.BlockedCheckPreset = GodfallPresets::SweepCollision;
		f3Dfilters.Add(&raycastFilter);

		// 캐릭터가 플레이어 캐릭터 뒤에 있는지 검사하기 위한 설정
		FCharacterManagerQueryAngleFilter angleFilter;
		angleFilter.Point = GetActorLocation();
		angleFilter.Direction = mCameraDirector->GetCamera()->GetForwardVector();
		angleFilter.MaxAngle = 50.f;
		f3Dfilters.Add(&angleFilter);

		FCharacterManagerQueryOption option;

		FCharacterManager3DQuery f3DQuery;
		f3DQuery.Point = GetActorLocation();
		f3DQuery.Rotation = mCameraDirector->GetCamera()->GetComponentRotation();

		AGodfallCharacterBase* character = gameState->GetCharacterManager()->Query(f3Dfilters, f3DQuery, option, nullptr);
		AGodfallEnemyBase* enemy = Cast<AGodfallEnemyBase>(character);
		if (enemy)
		{
			// Ignore attack input if succeeded execute
			if (TryExecute(enemy))
			{
				if (mCameraDirector->mFocusActor.IsValid())
				{
					mCameraDirector->mFocusActor = enemy;
				}

				return true;
			}
		}
	}

	return SetLightAttack();
}

bool ACPlayer::OnHeavyAttack()
{
	if (mAnimInstance->IsActiveExecutionMontage())
	{
		return true;
	}

	return SetHeavyAttack();
}

void ACPlayer::RotateToCameraForward()
{
	if (!ensure(mCameraDirector)) return;

	FVector camForward = FRotationMatrix(mCameraDirector->GetCamera()->GetComponentRotation()).GetUnitAxis(EAxis::X);
	camForward = FVector::VectorPlaneProject(camForward, FVector3d::UpVector);

	GetCharacterRotateComponent()->SetRotator(FRotationMatrix::MakeFromX(camForward).Rotator());
}

void ACPlayer::RotateToMoveDirection()
{
	if (!ensure(mCameraDirector)) return;

	if (mMoveDirection.SquaredLength() < DBL_EPSILON) return;

	FVector camForward = FRotationMatrix(mCameraDirector->GetCamera()->GetComponentRotation()).GetUnitAxis(EAxis::X);
	camForward = FVector::VectorPlaneProject(camForward, FVector3d::UpVector);

	FVector moveDirection = FVector(mMoveDirection.Y, mMoveDirection.X, 0.0f);
	moveDirection = FRotationMatrix::MakeFromX(camForward).TransformVector(moveDirection);

	GetCharacterRotateComponent()->SetRotator(FRotationMatrix::MakeFromX(moveDirection).Rotator());
}

void ACPlayer::AccAttackPressed()
{
	if (!mAttackPressed) return;

	mAttackPressedAcc += GetWorld()->GetDeltaSeconds() / UGameplayStatics::GetGlobalTimeDilation(GetWorld());
	
	if (mAttackPressedAcc >= mHeavyAttackAccTime && OnHeavyAttack())
	{
		mAttackPressed = false;
	}
}

void ACPlayer::ToggleLockOn()
{
	if (mCameraDirector->mFocusActor.IsValid())
	{
		mCameraDirector->mFocusActor = nullptr;
	}
	else
	{
		AGodfallGameState* gameState = Cast<AGodfallGameState>(GetWorld()->GetGameState());
		if (ensure(gameState))
		{
			TArray<const FCharacterManagerQueryFilter*> f3Dfilters;

			// 캐릭터가 벽에 가려져 있는지 검사하기 위한 레이캐스트 설정
			FCharacterManagerQueryRaycastFilter raycastFilter;
			raycastFilter.RaystartLocation = mCameraDirector->GetCamera()->GetComponentLocation();
			raycastFilter.World = GetWorld();
			raycastFilter.BlockedCheckPreset = GodfallPresets::SweepCollision;
			f3Dfilters.Add(&raycastFilter);

			// 캐릭터가 카메라 뒤에 있는지 검사하기 위한 설정
			FCharacterManagerQueryAngleFilter angleFilter;
			angleFilter.Point = mCameraDirector->GetCamera()->GetComponentLocation();
			angleFilter.Direction = mCameraDirector->GetCamera()->GetForwardVector();
			angleFilter.MaxAngle = 50.f;
			f3Dfilters.Add(&angleFilter);

			FCharacterManagerQueryOption option;

			// 캐릭터와 카메라 사이의 거리를 비교하기 위한 설정
			FCharacterManager3DQuery f3DQuery;
			f3DQuery.Point = mCameraDirector->GetCamera()->GetComponentLocation();
			f3DQuery.Rotation = mCameraDirector->GetCamera()->GetComponentRotation();

			// 벽에 가려져 있지 않은 캐릭터 중 화면 중심에서 가장 가까운 캐릭터를 탐색
			AGodfallCharacterBase* character = 
				gameState->GetCharacterManager()->Query(f3Dfilters, f3DQuery, option, nullptr);
			mCameraDirector->mFocusActor = character;		
		}
	}
}

void ACPlayer::TryChangeLockOnTarget(float deltaTime, const FVector2D& screenDelta)
{
#pragma region PRUNING
	if (mChangeLockOnDelay > 0.0f)
	{
		mChangeLockOnDelay -= deltaTime;
		return;
	}
	if (!mCameraDirector->mFocusActor.IsValid()) return;
#pragma endregion
	// 마우스 이동 거리가 한계값보다 작으면 락온 대상을 변경하지 않는다.
	if (screenDelta.Length() < mLimitDeltaLengthToChangeLockOn) return;
	AGodfallGameState* gameState = Cast<AGodfallGameState>(GetWorld()->GetGameState());
	if (ensure(gameState))
	{
#pragma region PARAMETER_SETTING
		TArray<const FCharacterManagerQueryFilter*> f3Dfilters;

		FCharacterManagerQueryDefaultFilter defaultFilter;
		defaultFilter.Ignores.Add(mCameraDirector->mFocusActor.Get());
		f3Dfilters.Add(&defaultFilter);

		FCharacterManagerQueryRaycastFilter raycastFilter;
		raycastFilter.RaystartLocation = mCameraDirector->GetCamera()->GetComponentLocation();
		raycastFilter.World = GetWorld();
		raycastFilter.BlockedCheckPreset = GodfallPresets::SweepCollision;
		f3Dfilters.Add(&raycastFilter);

		FCharacterManagerQueryOption option;

		FCharacterManager3DQuery f3DQuery;
		f3DQuery.Point = mCameraDirector->GetCamera()->GetComponentLocation();
		f3DQuery.Rotation = mCameraDirector->GetCamera()->GetComponentRotation();
#pragma endregion
		FCharacterManagerScreenQuery fScreenQuery;
		fScreenQuery.LineDirection = screenDelta;

		AGodfallCharacterBase* character = gameState->GetCharacterManager()->Query(f3Dfilters, f3DQuery, option, &fScreenQuery);
		
		if (character)
		{
			mChangeLockOnDelay = mDelayToChnageLockOn;
			mCameraDirector->mFocusActor = character;
		}
	}
}

void ACPlayer::UpdatePlayerHUD()
{
	AGodfallPlayerController* playerController = Cast<AGodfallPlayerController>(GetController());
	if (playerController)
	{
		UPlayerHUD* hud = playerController->GetPlayerHUD();
		if (hud)
		{
			hud->SetHealthPercent(GetHealth() / GetMaxHealth());
			hud->SetSteminaPercent(GetStemina() / GetMaxStemina());
		}
	}
}
