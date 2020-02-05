// Fill out your copyright notice in the Description page of Project Settings.

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "Platformer3DPlayerController.h"

void APlatformer3DPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDwidget)
	{
		PlayerHUD = CreateWidget<UUserWidget>(this, HUDwidget, TEXT("Player HUD"));

		if (PlayerHUD)
		{
			PlayerHUD->AddToViewport();
		}
	}

	/*** Maybe move to constructor ***/
	PlayerSaveGame = Cast<UPlatformer3DSaveGame>(UGameplayStatics::CreateSaveGameObject(UPlatformer3DSaveGame::StaticClass()));
}

bool APlatformer3DPlayerController::SaveGame(FSaveData SaveData)
{	
	if (PlayerSaveGame)
	{
		PlayerSaveGame->PlayerPosition = SaveData.PlayerPosition;
		PlayerSaveGame->CurrentHealth = SaveData.CurrentHealth;
		PlayerSaveGame->MaxHealth = SaveData.MaxHealth;
		PlayerSaveGame->BaseDamage = SaveData.BaseDamage;
		PlayerSaveGame->CurrentExp = SaveData.CurrentExp;
		PlayerSaveGame->ExpToNextLevel = SaveData.ExpToNextLevel;
		PlayerSaveGame->PlayerLevel = SaveData.PlayerLevel;
		PlayerSaveGame->SkillPoints = SaveData.SkillPoints;
		PlayerSaveGame->Skills = SaveData.Skills;
		PlayerSaveGame->ItemsHeld = SaveData.ItemsHeld;
		return UGameplayStatics::SaveGameToSlot(PlayerSaveGame, PlayerSaveGame->SaveSlotName, PlayerSaveGame->UserIndex);
	}

	return false;
}

bool APlatformer3DPlayerController::LoadGame(FSaveData& LoadedSaveData)
{
	if (PlayerSaveGame)
	{
		FString SlotName = PlayerSaveGame->SaveSlotName;
		uint32 PlayerIndex = PlayerSaveGame->UserIndex;
		UPlatformer3DSaveGame* LoadedGame = Cast<UPlatformer3DSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, PlayerIndex));
		if (LoadedGame)
		{
			LoadedSaveData.PlayerPosition = LoadedGame->PlayerPosition;
			LoadedSaveData.CurrentHealth = LoadedGame->CurrentHealth;
			LoadedSaveData.MaxHealth = LoadedGame->MaxHealth;
			LoadedSaveData.BaseDamage = LoadedGame->BaseDamage;
			LoadedSaveData.CurrentExp = LoadedGame->CurrentExp;
			LoadedSaveData.ExpToNextLevel = LoadedGame->ExpToNextLevel;
			LoadedSaveData.PlayerLevel = LoadedGame->PlayerLevel;
			LoadedSaveData.SkillPoints = LoadedGame->SkillPoints;
			LoadedSaveData.Skills = LoadedGame->Skills;
			LoadedSaveData.ItemsHeld = LoadedGame->ItemsHeld;
			return true;
		}
	}

	return false;
}