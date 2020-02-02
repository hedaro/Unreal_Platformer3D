// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Engine libraries
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
// Custom libraries
#include "Characters/PlayerCharacter.h"

#include "Platformer3DSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMER3D_API UPlatformer3DSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPlatformer3DSaveGame();

	UPROPERTY(VisibleAnywhere)
		FString SaveSlotName;

	UPROPERTY(VisibleAnywhere)
		uint32 UserIndex;

	UPROPERTY(EditAnywhere)
		FTransform PlayerPosition;

	UPROPERTY(EditAnywhere)
		float CurrentHealth;

	UPROPERTY(EditAnywhere)
		float MaxHealth;

	UPROPERTY(EditAnywhere)
		float BaseDamage;

	UPROPERTY(EditAnywhere)
		float CurrentExp;

	UPROPERTY(EditAnywhere)
		float ExpToNextLevel;

	UPROPERTY(EditAnywhere)
		int PlayerLevel;

	UPROPERTY(EditAnywhere)
		int SkillPoints;

	UPROPERTY(EditAnywhere)
		TMap<FString, FSkill> Skills;
};
