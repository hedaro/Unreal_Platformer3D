// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Engine libraries
#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "Components/WidgetComponent.h"
// Custom libraries
#include "Characters/Platformer3DCharacter.h"
#include "Enums/Item_Types.h"

#include "PlayerCharacter.generated.h"

USTRUCT(BlueprintType)
struct FSkill
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString SkillRequired;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Cost;

	UPROPERTY(VisibleAnywhere)
	bool Acquired;
};

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

	/*** Leveling ***/
	float CurrentExp;
	float ExpToNextLevel = 100.f;
	int PlayerLevel = 1;
	int SkillPoints = 1;

public:	
	/*** Camera Control ***/
	UPROPERTY(EditAnywhere, Category = Camera)
		FVector2D CameraZoomMinMax = FVector2D(100.0f, 400.0f);

	UPROPERTY(EditAnywhere, Category = Camera)
		float CameraZoomSpeed = 10.0f;

	/*** Movement ***/
	virtual void LookAt(FVector Location, float Rate) override;

	virtual void ReactToDamage(float AttackForce) override;

	/*** Attack ***/
	virtual void StartRangedAttack() override;

	virtual void EndRangedAttack() override;

	/*** Leveling ***/
	UFUNCTION(BlueprintCallable)
		void AddExp(float Experience);

	UFUNCTION(BlueprintCallable)
		void LevelUp();

	UFUNCTION(BlueprintPure)
		float GetCurrentExp() const;

	UFUNCTION(BlueprintPure)
		float GetExpToNextLevel() const;

	UFUNCTION(BlueprintPure)
		int GetCurrentLevel() const;

	/*** Skills System ***/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
	TMap<FString, FSkill> Skills;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
		TSubclassOf<class UUserWidget> SkillsMenuWidget;

	UPROPERTY()
	UUserWidget* SkillsMenu;

	UFUNCTION(BlueprintCallable)
		void ToggleSkillsMenu();

	UFUNCTION(BlueprintCallable)
		void ShowSkillsMenu();

	UFUNCTION(BlueprintCallable)
		void HideSkillsMenu();

	UFUNCTION(BlueprintCallable)
		int SpendSkillPoints(int Amount);

	UFUNCTION(BlueprintPure)
		int GetSkillPoints() const;

	UFUNCTION(BlueprintPure)
		FSkill GetSkill(FString SkillName) const;

	UFUNCTION(BlueprintPure)
		bool IsSkillAvailable(FString SkillName) const;

	UFUNCTION(BlueprintCallable)
		void AcquireSkill(FString SkillName);

	/*** Learnable Actions ***/
	virtual void StartDash() override;

	virtual void StartHeavyAttack() override;

	/*** Save/Load Game ***/
	UFUNCTION(BlueprintCallable)
	virtual void SaveGame();

	UFUNCTION(BlueprintCallable)
	virtual void LoadGame();

	/*** Items ***/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	TMap<EItem_Types, int> ItemsHeld;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Items")
		float HealthPotionRecovery = 100;

	UFUNCTION(BlueprintCallable)
		void AddPickUpItem(EItem_Types ItemType, int Amount);

	UFUNCTION(BlueprintPure)
		int GetPickUpItemNum(EItem_Types ItemType) const;

	UFUNCTION(BlueprintCallable)
		void UseHealthPotion();
};
