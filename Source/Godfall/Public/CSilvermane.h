// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GodfallCharacterBase.h"
#include "CSilvermaneEnums.h"
#include "GodfallUtil.h"
#include "CSilvermane.generated.h"

class AGodfallEnemyBase;
class AJumpArea;

/**
 * 
 */
UCLASS()
class GODFALL_API ACSilvermane : public AGodfallCharacterBase
{
	GENERATED_BODY()

public:
	ACSilvermane();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

	virtual bool CanRotate() const override;
	virtual bool IsInvincible() const override;
	virtual EBlockOutput CanBlock(const FDamageInput& damageInput) const override;
	virtual void OnGuard(const FDamageOutput& damageOutput) override;
	virtual void OnDamageImpl(const FDamageOutput& output, bool onZeroHealth, bool onMaxStemina) override;

	UFUNCTION()
	void OnCustomAnimNotifyState(EAnimNotifyStateEvent eEvent, const FAnimNotifiyStateData& data, const FName& notifyStateName, const FCustomNotifyStateVariable& variables);

public:
	void SetMove(FVector2D direction, ESilvermaneMoveSpeed speed);
	void Slide(E4Direction direction);
	void Slide(FVector2D direction);
	void SlideToCurrentDirection();
	void SetGuard(bool value);
	bool SetLightAttack();
	bool SetHeavyAttack();

	bool TrySetWeaponType(ESilvermaneWeapon value);
	ESilvermaneWeapon GetWeaponType() const;

	bool TryExecute(AGodfallEnemyBase* targetEnemy);

	bool TryJump();

private:
	void InitWeaponSocket();
	void InitWeaponMesh();
	void InitWeaponHitbox();
	void SetWeaponSocket(ESilvermaneWeaponSocket value);
	ESilvermaneWeaponSocket GetWeaponSocket() const;
	void SetWeaponVisibility(ESilvermaneWeapon value);
	void SetWeaponHitbox(ESilvermaneWeapon value);
	void SetWeaponType(ESilvermaneWeapon value, bool withAnimation);

	void OnBeginSprint();
	void OnEndSprint();
	void OnEquippedOff();
	void OnEquippedOn();
	void OnAttack();
	void OnShield();
	void OnBeginExecution();
	void OnEndExecution(bool interruped);
	void OnJump();
	void OnJumpingSection();

	void TickForShield();
	void TickEquipCheck();
	void TickForJump();
	void TickForExecuteLocation();

protected:
	UPROPERTY()
	class UAnimInstanceSilvermane* mAnimInstance;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	USceneComponent* WeaponLSocket;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	USceneComponent* WeaponRSocketComponent;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	USceneComponent* Sword1HBackSocket;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Sword1HMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	UCapsuleComponent* Sword1HHitbox;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	USceneComponent* Sword2HBackSocket;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* Sword2HMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	UCapsuleComponent* Sword2HHitbox;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	USceneComponent* PolearmBackSocket;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent * PolearmMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	UCapsuleComponent* PolearmHitbox;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	USceneComponent* DualBladesLeftBackSocket;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent * DualbladesLeftMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	UCapsuleComponent* DualbladesLeftHitbox;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	USceneComponent* DualBladesRightBackSocket;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent * DualbladesRightMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	UCapsuleComponent* DualbladesRightHitbox;

	UPROPERTY(VisibleAnywhere, DisplayName = "Shield", Category = "Weapon", Meta = (AllowPrivateAccess = true))
	class AAShield* mShield;

	UPROPERTY()
	AGodfallEnemyBase* mExecutioningTarget;

	UPROPERTY(EditAnywhere, DisplayName = "Perfect Block Limit", Category = "Actor", Meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float mPerfectBlockLimit = 0.25f;

	FVector2D mMoveDirection = FVector2D(0.0f, 1.0f);
	ESilvermaneMoveSpeed mMoveSpeed = ESilvermaneMoveSpeed::Stop;
	bool mGuard = false;
	float mLastGuardTime = 0.0f;

	bool mShieldOverride = false;
	bool mShieldOverrideOpen = false;

	float mExecuteDistance = 300.0f;

	FParabolaCoefficient3 mParabolaJumpCoefficient;
};
