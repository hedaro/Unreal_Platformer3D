// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLATFORMER3D_API UHealthActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthActorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	/** MY BEHAVIOUR **/
private:
	UPROPERTY(VisibleAnywhere)
		bool Alive = true;

	UPROPERTY(VisibleAnywhere)
		float CurrentHealth;

public:
	UPROPERTY(EditAnywhere)
		float MaxHealth = 100;

	UFUNCTION(BlueprintPure)
		float GetCurrentHealth();

	UFUNCTION(BlueprintPure)
		bool IsAlive();

	UFUNCTION(BlueprintCallable)
		void DecreaseHealth(float Amount);

	UFUNCTION(BlueprintCallable)
		void RecoverHealth(float Amount);

};
