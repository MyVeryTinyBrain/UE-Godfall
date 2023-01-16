// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraDirector.h"

ACameraDirector::ACameraDirector()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	SpringArm->TargetArmLength = 520.0f;
	SpringArm->SocketOffset = FVector(100.0f, 85.0f, 45.0f);
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagSpeed = 20.0f;
	SpringArm->CameraRotationLagSpeed = 20.0f;

	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritRoll = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bDoCollisionTest = true;
}

void ACameraDirector::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ACameraDirector::BeginPlay()
{
	Super::BeginPlay();
}

void ACameraDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FollowTargetActor();
	RotateSpringArm();
}

void ACameraDirector::InputTurnRightLeft(float input)
{
	mCameraDeltaRotation.Yaw = input;
}

void ACameraDirector::InputLookUpDown(float input)
{
	mCameraDeltaRotation.Pitch = -input;
}

FVector ACameraDirector::GetFocusLocation() const
{
	FVector location = Camera->GetComponentLocation();
	switch (mFocusType)
	{
		case ECameraDirectorFocus::Actor:
		{
			if (!mFocusActor.IsValid()) break;

			ACharacter* actorAsCharacter = Cast<ACharacter>(mFocusActor);

			if (actorAsCharacter && 
				actorAsCharacter->GetCapsuleComponent() &&
				actorAsCharacter->GetMesh() && 
				INDEX_NONE != actorAsCharacter->GetMesh()->GetBoneIndex(TEXT("pelvis")))
			{
				location = FVector::ZeroVector 
					+ actorAsCharacter->GetMesh()->GetBoneLocation(TEXT("pelvis")) 
					/*+ FVector::DownVector * actorAsCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()*/;
			}
			else
			{
				location = mFocusActor->GetActorLocation();
			}
		}
		break;
		case ECameraDirectorFocus::Location:
		{
			location = mFocustLocation;
		}
		break;
	}
	return location;
}

FVector ACameraDirector::GetFocusDirection() const
{
	FVector direction = Camera->GetForwardVector();

	switch (mRotateMode)
	{
		case ECameraDirectorRotate::Focus:
		case ECameraDirectorRotate::FocusOrInput:
		{
			if (mFocusType == ECameraDirectorFocus::Actor && mFocusActor.IsValid())
			{
				direction = GetFocusLocation() - GetActorLocation();
			}
		}
		break;
	}

	return FVector::VectorPlaneProject(direction, FVector3d::UpVector).GetSafeNormal();
}

void ACameraDirector::FollowTargetActor()
{
	if (mTargetActor.IsValid())
	{
		Follow(mTargetActor->GetActorLocation());
	}
}

void ACameraDirector::Follow(const FVector& location)
{
	SetActorLocation(location);
}

void ACameraDirector::RotateSpringArm()
{
	switch (mRotateMode)
	{
		case ECameraDirectorRotate::None:
		{
			SpringArm->CameraRotationLagSpeed = mInputRotationLagSpeed;
		}
		break;
		case ECameraDirectorRotate::Input:
		{
			ApplyRotateInput();
			SpringArm->CameraRotationLagSpeed = mInputRotationLagSpeed;
		}
		break;
		case ECameraDirectorRotate::Focus:
		{
			LookFocusTarget();
			SpringArm->CameraRotationLagSpeed = mLockonRotationLagSpeed;
		}
		break;
		case ECameraDirectorRotate::FocusOrInput:
		{
			if (mFocusType == ECameraDirectorFocus::Actor && mFocusActor.IsValid())
			{
				LookFocusTarget();
				SpringArm->CameraRotationLagSpeed = mLockonRotationLagSpeed;
			}
			else
			{
				ApplyRotateInput();
				SpringArm->CameraRotationLagSpeed = mInputRotationLagSpeed;
			}
		}
		break;
	}
}

void ACameraDirector::ApplyRotateInput()
{
	FRotator springArmRotation = SpringArm->GetComponentRotation();
	FRotator addedRotation = springArmRotation.Add(mCameraDeltaRotation.Pitch, mCameraDeltaRotation.Yaw, 0.0f);
	FRotator newRotation = FRotator(FMath::Clamp(addedRotation.Pitch, -89.0f, 89.0f), addedRotation.Yaw, addedRotation.Roll);
	SpringArm->SetWorldRotation(newRotation);

	mCameraDeltaRotation = FRotator::ZeroRotator;
}

void ACameraDirector::LookFocusTarget()
{
	FVector location = GetFocusLocation();

	Look(location + mTargetOffset);
}

void ACameraDirector::Look(const FVector& location)
{
	//FRotator newRotation = FRotationMatrix::MakeFromX(location - Camera->GetComponentLocation()).Rotator();
	FRotator newRotation = FRotationMatrix::MakeFromX(location - GetActorLocation()).Rotator();
	SpringArm->SetWorldRotation(newRotation);
}

