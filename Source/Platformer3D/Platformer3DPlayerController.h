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
