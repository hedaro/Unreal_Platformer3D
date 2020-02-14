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

	UPROPERTY(VisibleAnywhere)
		FTransform PlayerPosition;

	UPROPERTY(VisibleAnywhere)
		float CurrentHealth;

	UPROPERTY(VisibleAnywhere)
		float MaxHealth;

	UPROPERTY(VisibleAnywhere)
		float BaseDamage;

	UPROPERTY(VisibleAnywhere)
		float CurrentExp;

	UPROPERTY(VisibleAnywhere)
		float ExpToNextLevel;

	UPROPERTY(VisibleAnywhere)
		int PlayerLevel;

	UPROPERTY(VisibleAnywhere)
		int SkillPoints;

	UPROPERTY(VisibleAnywhere)
		TMap<ESkills, FSkill> Skills;

	UPROPERTY(VisibleAnywhere)
		TMap<EItem_Types, int> ItemsHeld;
};
