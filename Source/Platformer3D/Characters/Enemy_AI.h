// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Engine libraries
#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Engine/TargetPoint.h"
// Custom libraries
#include "Characters/Platformer3DCharacter.h"
#include "Characters/PlayerCharacter.h"
#include "Interfaces/LockOn_Interface.h"
#include "Widgets/EnemyGUI_Widget.h"
#include "Items/PickUpItem.h"

#include "Enemy_AI.generated.h"

UCLASS()
class PLATFORMER3D_API AEnemy_AI : public APlatformer3DCharacter, public ILockOn_Interface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy_AI();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	/** MY BEHAVIOUR **/
	UFUNCTION()
		void OnSeePawn(APawn* OtherPawn);

	UFUNCTION()
		void SeekPlayer();

	UFUNCTION()
		void MoveToTargetPoint();

private:
	/*** AI ***/
	APlayerCharacter* PlayerCharacter;
	bool SeenPlayer = false;
	bool IsWaiting = false;
	float WaitTimer = 0.f;

	int CurrentTargetPointIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	TArray<ATargetPoint*> TargetPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
		UPawnSensingComponent* PawnSensingComponent;

	UFUNCTION()
	float DistanceToPlayer();
	FTimerHandle DeathTimerHandle;

	/*** Combat ***/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* GUIWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		UBoxComponent* AssassinationArea;

	UFUNCTION()
		void DropLoot();

	/*** Temporary Combat variables (Need refactor) ***/
	int CurrentCombo;

	UEnemyGUI_Widget* GUIWidget;

	/*** Leveling ***/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Leveling", meta = (AllowPrivateAccess= "true"))
		float ExpGiven = 60.f;

public:
	UPROPERTY(EditAnywhere, Category = "AI")
		FVector2D DistanceToPlayerMinMax;

	UPROPERTY(EditAnywhere, Category = "AI")
		float TargetPointDistanceTolerance = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
		TArray<TSubclassOf<APickUpItem>> LootList;

	virtual void DoDamage(AActor* Target) override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	virtual void ReactToDamage(float AttackForce) override;

	UFUNCTION(BlueprintCallable)
		void Despawn();

	UFUNCTION(BlueprintCallable)
		void Wait(float Seconds);

	UFUNCTION(BlueprintCallable)
		void CancelWait();

	/*** LockOn Interface ***/
	UFUNCTION(BlueprintCallable)
	virtual void ClearLockOn() override;

	UFUNCTION(BlueprintCallable)
	virtual bool IsValidTarget() override;
};
