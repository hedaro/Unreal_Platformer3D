// Fill out your copyright notice in the Description page of Project Settings.

#include "FloatingText_Widget.h"

#include "Kismet/KismetMathLibrary.h"

void UFloatingText_Widget::SetUpWidget(FText DisplayText, FVector2D InitialPosition, float FadeTime)
{
	Text = DisplayText;
	CurrentPosition = InitialPosition;
	float Xoffset = FMath::RandRange(-150.f, 150.f);
	float Yoffset = FMath::RandRange(-150.f, 150.f);
	FinalPosition = InitialPosition + FVector2D(Xoffset, Yoffset);
	LifeTime = FadeTime;

	AddToViewport();
}

void UFloatingText_Widget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::Tick(MyGeometry, DeltaTime);

	CurrentPosition = UKismetMathLibrary::Vector2DInterpTo(CurrentPosition, FinalPosition, DeltaTime, 0.5);
	SetPositionInViewport(CurrentPosition);

	ElapsedTime = FMath::Clamp(ElapsedTime + DeltaTime, 0.f, LifeTime);

	if (ElapsedTime >= LifeTime)
	{
		RemoveFromParent();
	}
}

float UFloatingText_Widget::GetTimeLeftPercent() const
{
	if (LifeTime > 0.f)
	{
		return ElapsedTime / LifeTime;
	}

	return 1.f;
}

FText UFloatingText_Widget::GetText() const
{
	return Text;
}