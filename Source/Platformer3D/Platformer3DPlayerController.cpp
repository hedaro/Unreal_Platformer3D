// Fill out your copyright notice in the Description page of Project Settings.


#include "Platformer3DPlayerController.h"

#include "Blueprint/UserWidget.h"

void APlatformer3DPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDwidget)
	{
		PlayerHUD = CreateWidget<UUserWidget>(this, HUDwidget);

		if (PlayerHUD)
		{
			PlayerHUD->AddToViewport();
		}
	}
}