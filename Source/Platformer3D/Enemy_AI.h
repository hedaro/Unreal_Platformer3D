// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Platformer3DCharacter.h"
#include "LockOn_Interface.h"
#include "Perception/PawnSensingComponent.h"

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

private:
	/*** AI ***/
	bool SeenPlayer = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UPawnSensingComponent* PawnSensingComponent;

public:

};
