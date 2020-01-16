// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Engine/EngineTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Public/TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "LockOn_Interface.h"
#include "Animation/AnimMontage.h"
#include "Components/PrimitiveComponent.h"
#include "HealthActorComponent.h"

#include "Platformer3DCharacter.generated.h"

UCLASS(config=Game)
class PLATFORMER3D_API APlatformer3DCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	APlatformer3DCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Landed(const FHitResult& Hit) override;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface


	/** MY BEHAVIOUR **/
protected:
	/*** Movement ***/
	UFUNCTION()
		void DisablePlayerMoveInput();

	UFUNCTION()
		void EnablePlayerMoveInput();

	void StartJump();

	void EndJump();

	void ZoomCamera(float Value);

	UFUNCTION()
		void StartDash();

	UFUNCTION()
		void EndDash();

	UFUNCTION()
		void ResetDash();

	UFUNCTION()
		void StartRun();

	UFUNCTION()
		void EndRun();

	UFUNCTION()
		void ToggleCameraMode();

	UFUNCTION()
		void RollDodge(FKey Key);

	UFUNCTION()
		void ExecuteRollDodge();

	UFUNCTION()
		void ResetRollDodgeAnimation();

	UFUNCTION()
		void LockOnTarget();

	UFUNCTION()
		void LockOffTarget();

	UFUNCTION()
		void StartAttack();

	UFUNCTION()
		void EndAttackLaunch();

	UFUNCTION()
		void StopAttackMontage();

private:
	/*** Movement ***/
	UCharacterMovementComponent* CharacterMovementComponent;
	FVector HorizontalSpeed;

	/*** Player controller ***/
	APlayerController* PlayerController;
	FKey KeyBuffer;

	/*** Camera Control ***/
	int CameraMode = 0;

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

	/*** Lock On ***/
	bool TargetLocked;
	AActor* NearestTarget;

	/*** Attack ***/
	bool IsAttacking = false;
	bool SaveAttack = false;
	int ComboCount = 0;
	float AttackCooldown = 0.f;
	float LaunchForce = 0.f;
	float JumpForce = 0.f;
	//UAnimInstance* AnimInstance; // WHY CANT I JUST COMMENT THIS!!!!!!!!!!!!!!!!!!!!!!!!
	FTimerHandle AttackTimerHandle;

	/*** Combat ***/
	UPrimitiveComponent* AttackHitbox;
	UHealthActorComponent* HealthComponent;


public:
	/*** Camera Control ***/
	UPROPERTY(EditAnywhere, Category=Camera)
		FVector2D CameraZoomMinMax = FVector2D(100.0f, 400.0f);

	UPROPERTY(EditAnywhere, Category = Camera)
		float CameraZoomSpeed = 10.0f;

	/*** Jump ***/
	UPROPERTY(EditAnywhere, Category = Movement)
		float FallSpeedRatio = 3.5;
	
	UPROPERTY(EditAnywhere, Category = Movement)
		float FastFallSpeedRatio = 4.5;

	UPROPERTY(EditAnywhere, Category = Movement)
		int MaxJumpCount = 1;

	/*** Dash ***/
	UPROPERTY(EditAnywhere, Category = Movement)
		float DashSpeed = 6000.0f;

	UPROPERTY(EditAnywhere, Category = Movement)
		float DashDuration = 0.1f;

	UPROPERTY(EditAnywhere, Category = Movement)
		float DashCooldownTime = 1.f;

	UFUNCTION(BlueprintPure)
		bool GetIsDashing();

	/*** Run/Walk ***/
	UPROPERTY(EditAnywhere, Category = Movement)
		float RunSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = Movement)
		float WalkSpeed = 300.f;

	/*** Roll/Dodge ***/
	UFUNCTION(BlueprintPure)
		int GetRollDodgeAnimation();

	/*** LockOn ***/
	UPROPERTY(EditAnywhere, Category = LockOn)
		float MinimumDistanceToTarget = 2000;

	/*** Attack ***/
	UFUNCTION(BlueprintCallable)
		void SaveComboAttack();

	UFUNCTION(BlueprintCallable)
		void ResetCombo();

	UFUNCTION(BlueprintCallable)
		void ApplyAttackLaunch();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		TArray<UAnimMontage*> AttackMontages;

	/*** Combat ***/
	UFUNCTION(BlueprintCallable)
		void RegisterAttackHitbox(UPrimitiveComponent* Hitbox);

	UFUNCTION(BlueprintCallable)
		void EnableAttackHitBox();

	UFUNCTION(BlueprintCallable)
		void DisableAttackHitBox();
};

