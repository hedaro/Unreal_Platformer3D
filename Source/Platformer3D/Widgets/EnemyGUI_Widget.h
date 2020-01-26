// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Engine libraries
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
// Custom libraries
#include "Characters/Platformer3DCharacter.h"

#include "EnemyGUI_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMER3D_API UEnemyGUI_Widget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		APlatformer3DCharacter* OwnerNPC;

	UFUNCTION()
		void SetOwnerNPC(APlatformer3DCharacter* Character);
};
