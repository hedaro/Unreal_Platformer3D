// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

// Engine libraries
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/EngineTypes.h"
#include "Engine/Public/TimerManager.h"
#include "GameFramework/Controller.h"
#include "Animation/AnimMontage.h"
#include "Components/ShapeComponent.h"
// Custom libraries
#include "Components/AttackSystemComponent.h"
#include "Components/HealthActorComponent.h"
#include "Interfaces/DamagableObject_Interface.h"

#include "Platformer3DCharacter.generated.h"

UCLASS(config=Game)
class PLATFORMER3D_API APlatformer3DCharacter : public ACharacter, public IDamagableObject_Interface
{
	GENERATED_BODY()

public:
	APlatformer3DCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	virtual void Tick(float DeltaTime) override;

	virtual void Landed(const FHitResult& Hit) override;

protected:
	virtual void BeginPlay() override;

	/** MY BEHAVIOUR **/

	/*** Movement ***/
	FVector HorizontalSpeed;

	UFUNCTION()
		void DisableMoveInput();

	UFUNCTION()
		void EnableMoveInput();

	UFUNCTION()
		void DisableLookupInput();

	UFUNCTION()
		void EnableLookupMoveInput();

	/*** Lock On ***/
	bool TargetLocked;
	AActor* NearestTarget;

	/*** Jump ***/
	int JumpCount = 0;

	/*** Dash ***/
	bool CanDash = true;
	bool DashedOnAir = false;
	bool IsDashing = false;
	FTimerHandle DashTimerHandle;

	/*** Roll/Dodge ***/
	FTimerHandle RollTimerHandle;
	int RollDodgeAction = 0;
	int RollDodgeAnimation = 0;

	/*** Attack ***/
	FTimerHandle AttackTimerHandle;

	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
		UAttackSystemComponent* AttackSystem;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* WeaponMesh;

	/*** Combat ***/
	UShapeComponent* AttackHitbox;
	FTimerHandle DamageTimerHandle;
	bool IsFlinching = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
		UHealthActorComponent* HealthComponent;

	UFUNCTION()
		void EndReactToDamage();

	/*** Stun ***/
	bool IsStunned = false;
	float StunGauge = 0.f;

public:
	/*** Movement ***/
	/** Called via input to turn at a given rate. */
	void TurnAtRate(float Rate);

	/** Called via input to turn look up/down at a given rate. */
	void LookUpAtRate(float Rate);

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void ResetMoveState();

	UFUNCTION()
		void SetCrouchState(bool DoCrouch);

	UFUNCTION()
		void ToggleCrouchState();

	UFUNCTION(BlueprintPure)
		bool GetIsCrouching();

	/*** Camera ***/
	UFUNCTION()
		virtual void LookAt(FVector Location, float Rate);

	/*** Jump ***/
	UPROPERTY(EditAnywhere, Category = Movement)
		float FallSpeedRatio = 3.5;
	
	UPROPERTY(EditAnywhere, Category = Movement)
		float FastFallSpeedRatio = 4.5;

	UPROPERTY(EditAnywhere, Category = Movement)
		int MaxJumpCount = 1;

	UFUNCTION()
		virtual void StartJump();

	UFUNCTION()
		void EndJump();

	/*** Dash ***/
	UPROPERTY(EditAnywhere, Category = Movement)
		float DashSpeed = 6000.0f;

	UPROPERTY(EditAnywhere, Category = Movement)
		float DashDuration = 0.1f;

	UPROPERTY(EditAnywhere, Category = Movement)
		float DashCooldownTime = 1.f;

	UFUNCTION()
		virtual void StartDash();

	UFUNCTION()
		void EndDash();

	UFUNCTION()
		void ResetDash();

	UFUNCTION(BlueprintPure)
		bool GetIsDashing();

	/*** Run/Walk ***/
	UPROPERTY(EditAnywhere, Category = Movement)
		float RunSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = Movement)
		float WalkSpeed = 300.f;

	UFUNCTION()
		virtual void StartRun();

	UFUNCTION()
		void EndRun();

	/*** Roll/Dodge ***/
	UFUNCTION(BlueprintPure)
		int GetRollDodgeAnimation();

	UFUNCTION()
		virtual void RollDodge();

	UFUNCTION()
		virtual void ExecuteRollDodge();

	UFUNCTION()
		void ResetRollDodgeAnimation();

	/*** LockOn ***/
	UPROPERTY(EditAnywhere, Category = LockOn)
		float MinimumDistanceToTarget = 2000;

	UFUNCTION()
		virtual void LockOnTarget(AActor* Target);

	UFUNCTION()
		virtual void LockOffTarget();

	/*** Attack ***/
	UFUNCTION(BlueprintCallable)
		virtual void StartAttack();

	UFUNCTION(BlueprintCallable)
		virtual void StartHeavyAttack();

	UFUNCTION(BlueprintCallable)
		virtual void EndAttack();

	UFUNCTION(BlueprintCallable)
		void SaveCombo();
	
	UFUNCTION(BlueprintCallable)
		void ApplyAttackLaunch();

	UFUNCTION(BlueprintCallable)
		void EndAttackLaunch();

	UFUNCTION(BlueprintCallable)
		virtual void StartRangedAttack();

	UFUNCTION(BlueprintCallable)
		virtual void EndRangedAttack();

	/*** Combat ***/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
		float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* DamageMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* DeathMontage;

	UFUNCTION(BlueprintCallable)
		void RegisterAttackHitbox(UShapeComponent* Hitbox);

	UFUNCTION(BlueprintCallable)
		void RegisterWeaponMesh(UStaticMeshComponent* MeshComponent);

	UFUNCTION(BlueprintCallable)
		void EnableAttackHitBox();

	UFUNCTION(BlueprintCallable)
		void DisableAttackHitBox();

	UFUNCTION(BlueprintCallable)
		virtual void OnAttackOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
		virtual void DoDamage(AActor* Target);

	UFUNCTION()
		virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	UFUNCTION()
		virtual void ReactToDamage(float AttackForce) override;

	UFUNCTION(BlueprintPure)
		float GetCurrentHealth() const;

	UFUNCTION(BlueprintPure)
		float GetCurrentHealthPercent() const;

	UFUNCTION(BlueprintPure)
		bool GetIsAlive() const override;

	/*** Stun ***/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunResistance = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunRecoveryRate = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* StunMontage;

	UFUNCTION(BlueprintPure)
		float GetStunGauge() const;
};

