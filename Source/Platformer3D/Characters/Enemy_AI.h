// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Engine libraries
#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Perception/PawnSensingComponent.h"
// Custom libraries
#include "Characters/Platformer3DCharacter.h"
#include "Characters/PlayerCharacter.h"
#include "Interfaces/LockOn_Interface.h"
#include "Widgets/EnemyGUI_Widget.h"

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

private:
	/*** AI ***/
	APlayerCharacter* PlayerCharacter;
	bool SeenPlayer = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
		UPawnSensingComponent* PawnSensingComponent;

	/*** Combat ***/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		UWidgetComponent* GUIWidgetComponent;

	UEnemyGUI_Widget* GUIWidget;

	bool Flinch;

public:
	UPROPERTY(EditAnywhere, Category = AI)
		FVector2D DistanceToPlayerMinMax;

	virtual void DoDamage(AActor* Target) override;

	virtual void ReactToDamage() override;

	float DistanceToPlayer();
};
