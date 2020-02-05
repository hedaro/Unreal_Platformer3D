// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// EngineLibraries
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
//Custom libraries
#include "Platformer3DSaveGame.h"

#include "Platformer3DPlayerController.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FSaveData
{
	GENERATED_BODY()

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
		TMap<FString, FSkill> Skills;

	UPROPERTY(VisibleAnywhere)
		TMap<EItem_Types, int> ItemsHeld;
};

UCLASS()
class PLATFORMER3D_API APlatformer3DPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UUserWidget> HUDwidget;

	UUserWidget* PlayerHUD;

	virtual void BeginPlay() override;

	/*** Save/Load Game ***/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UPlatformer3DSaveGame* PlayerSaveGame;

	UFUNCTION(BlueprintCallable)
	bool SaveGame(FSaveData SaveData);

	UFUNCTION(BlueprintCallable)
	bool LoadGame(FSaveData& LoadedSaveData);
};
