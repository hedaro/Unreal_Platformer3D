// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Platformer3DCharacter.h"

#include "InputCoreTypes.h"

#include "PlayerCharacter.generated.h"

UCLASS()
class PLATFORMER3D_API APlayerCharacter : public APlatformer3DCharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** MY BEHAVIOUR **/
	/*** Camera ***/
	void ZoomCamera(float Value);

	UFUNCTION()
	void ToggleCameraMode();

	UFUNCTION()
	void FindNearestTarget();

	/*** Movement ***/
	/***** Find a way to remove dependance on specific keys!!!!!!!!!! *****/
	void RollDodgeInput(FKey Key);

	virtual void RollDodge() override;

	virtual void ExecuteRollDodge() override;

private:
	/*** Movement ***/
	FKey KeyBuffer;

	/*** Camera Control ***/
	int CameraMode = 0;

public:	
	/*** Camera Control ***/
	UPROPERTY(EditAnywhere, Category = Camera)
		FVector2D CameraZoomMinMax = FVector2D(100.0f, 400.0f);

	UPROPERTY(EditAnywhere, Category = Camera)
		float CameraZoomSpeed = 10.0f;

	/*** Movement ***/
	virtual void LookAt(FVector Location, float Rate) override;

	virtual void ReactToDamage() override;
};
