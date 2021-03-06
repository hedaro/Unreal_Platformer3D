// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthActorComponent.h"

// Sets default values for this component's properties
UHealthActorComponent::UHealthActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CurrentHealth = MaxHealth;
	Alive = true;
}


// Called every frame
void UHealthActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UHealthActorComponent::GetMaxHealth() const
{
	return MaxHealth;
}

void UHealthActorComponent::SetMaxHealth(float Health)
{
	MaxHealth = Health;
}

float UHealthActorComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

void UHealthActorComponent::SetCurrentHealth(float Health)
{
	CurrentHealth = Health;
}

bool UHealthActorComponent::IsAlive() const
{
	return Alive;
}

void UHealthActorComponent::DecreaseHealth(float Amount)
{
	CurrentHealth = FMath::Clamp<float>(CurrentHealth - Amount, 0, MaxHealth);
	Alive = CurrentHealth > 0;
}

void UHealthActorComponent::RecoverHealth(float Amount)
{
	CurrentHealth = FMath::Clamp<float>(CurrentHealth + Amount, 0, MaxHealth);
}