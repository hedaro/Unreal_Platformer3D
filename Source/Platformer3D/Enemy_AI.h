// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "LockOn_Interface.h"
#include "HealthActorComponent.h"

#include "Enemy_AI.generated.h"

UCLASS()
class PLATFORMER3D_API AEnemy_AI : public ACharacter, public ILockOn_Interface
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

	/*** Combat ***/
	UFUNCTION()
		virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	UFUNCTION()
		void ReactToDamage();

private:
	/*** Movement ***/
	UCharacterMovementComponent* CharacterMovementComponent;

	/*** Player controller ***/
	AController* AIController;

	/*** Combat ***/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
		UHealthActorComponent* HealthComponent;

	UShapeComponent* AttackHitbox;

public:	
	/*** Combat ***/
	/*UFUNCTION(BlueprintCallable)
		void RegisterAttackHitbox(UShapeComponent* Hitbox);

	UFUNCTION(BlueprintCallable)
		void OnAttackOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
		void EnableAttackHitBox();

	UFUNCTION(BlueprintCallable)
		void DisableAttackHitBox();*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* DamageMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		UAnimMontage* DeathMontage;

};
