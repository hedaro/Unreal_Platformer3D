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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	/*** Combat ***/
	UShapeComponent* AttackHitbox;
	UHealthActorComponent* HealthComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/*virtual void LockOn() override;

	virtual float GetDistanceFromPlayer() override;

	virtual float GetDistanceFrom(AActor* OtherActor) override;*/
};
