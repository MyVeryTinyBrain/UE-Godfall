// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Godfall.h"
#include "GameFramework/Actor.h"
#include "CameraDirector.generated.h"

UENUM(BlueprintType)
enum class ECameraDirectorRotate : uint8
{
	None,
	Input,
	Focus,

	// 포커스 타입이 액터일 때, 액터가 존재한다면 포커스 모드를 사용하고
	// 액터가 존재하지 않는다면 인풋 모드를 사용합니다.
	FocusOrInput, 
};

UENUM(BlueprintType)
enum class ECameraDirectorFocus : uint8
{
	Actor,
	Location,
};

UCLASS()
class GODFALL_API ACameraDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	ACameraDirector();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	void InputTurnRightLeft(float input);
	void InputLookUpDown(float input);

	FVector GetFocusLocation() const;
	FVector GetFocusDirection() const;

	UCameraComponent* GetCamera() const { return Camera; }
	USpringArmComponent* GetSpringArm() const { return SpringArm; }

private:
	void FollowTargetActor();
	void Follow(const FVector& location);

	void RotateSpringArm();
	void ApplyRotateInput();
	void LookFocusTarget();
	void Look(const FVector& location);

public:
	UPROPERTY(EditAnywhere, DisplayName = "Use Follow", Category = "Actor")
	bool mUesFollow = true;

	UPROPERTY(EditAnywhere, DisplayName = "Target Actor", Category = "Actor", Meta = (EditCondition = "mUesFollow == true", EditConditionHides))
	TWeakObjectPtr<AActor> mTargetActor;

	UPROPERTY(EditAnywhere, DisplayName = "Rotate Mode", Category = "Actor")
	ECameraDirectorRotate mRotateMode = ECameraDirectorRotate::FocusOrInput;

	UPROPERTY(EditAnywhere, DisplayName = "Input Rotate Factor", Category = "Actor", Meta = (EditCondition = "mRotateMode == ECameraDirectorRotate::Input", EditConditionHides))
	float mInputRotateFactor = 1.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Focus Type", Category = "Actor", Meta = (EditCondition = "(mRotateMode == ECameraDirectorRotate::Focus || mRotateMode == ECameraDirectorRotate::FocusOrInput)", EditConditionHides))
	ECameraDirectorFocus mFocusType = ECameraDirectorFocus::Actor;

	UPROPERTY(EditAnywhere, DisplayName = "Focus Actor", Category = "Actor", Meta = (EditCondition = "(mRotateMode == ECameraDirectorRotate::Focus || mRotateMode == ECameraDirectorRotate::FocusOrInput) && mFocusType = ECameraDirectorFocus::Actor", EditConditionHides))
	TWeakObjectPtr<AActor> mFocusActor;

	UPROPERTY(EditAnywhere, DisplayName = "Focus Location", Category = "Actor", Meta = (EditCondition = "(mRotateMode == ECameraDirectorRotate::Focus || mRotateMode == ECameraDirectorRotate::FocusOrInput) && mFocusType = ECameraDirectorFocus::Location", EditConditionHides))
	FVector mFocustLocation;

	UPROPERTY(EditAnywhere, DisplayName = "Target Offset", Category = "Actor", Meta = (EditCondition = "(mRotateMode == ECameraDirectorRotate::Focus || mRotateMode == ECameraDirectorRotate::FocusOrInput)", EditConditionHides))
	FVector mTargetOffset = FVector(0.0f, 0.0f, -50.0f);

	UPROPERTY(EditAnywhere, DisplayName = "Input Rotation Lag Speed", Category = "Actor")
	float mInputRotationLagSpeed = 20.0f;

	UPROPERTY(EditAnywhere, DisplayName = "Lockon Rotation Lag Speed", Category = "Actor")
	float mLockonRotationLagSpeed = 5.0f;

private:
	UPROPERTY(VisibleAnywhere, Category = "Actor", Meta = (AllowPrivateAccess = true))
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, Category = "Actor", Meta = (AllowPrivateAccess = true))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Actor", Meta = (AllowPrivateAccess = true))
	UCameraComponent* Camera;

	FRotator mCameraDeltaRotation = FRotator::ZeroRotator;
};
