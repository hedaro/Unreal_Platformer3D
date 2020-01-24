// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Platformer3DPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMER3D_API APlatformer3DPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UUserWidget> HUDwidget;

	UUserWidget* PlayerHUD;

	virtual void BeginPlay() override;
};
