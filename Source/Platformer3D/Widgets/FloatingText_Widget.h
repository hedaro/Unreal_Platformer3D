// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FloatingText_Widget.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMER3D_API UFloatingText_Widget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	FText Text;

	FVector2D CurrentPosition;
	float LifeTime;
	float ElapsedTime;

	FVector2D FinalPosition;

public:
	//virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void SetUpWidget(FText DisplayText, FVector2D InitialPositino, float FadeTime);

	UFUNCTION(BlueprintPure)
		float GetTimeLeftPercent() const;

	UFUNCTION(BlueprintPure)
		FText GetText() const;
};
