// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Engine libraries
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
// Custom libraries
#include "Enums/Skills.h"

#include "BurstSkillBase.generated.h"

// This class does not need to be modified.
UCLASS()
class PLATFORMER3D_API ABurstSkillBase : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
private:
	bool Initialized;
	bool Active;
	ESkills SkillType;

	UPROPERTY(EditAnywhere)
		float Duration;

	UPROPERTY(EditAnywhere)
		float Damage;

public:
	UFUNCTION(BlueprintCallable)
		void Activate();

	UFUNCTION(BlueprintImplementableEvent)
		void OnActivate();

	UFUNCTION(BlueprintCallable)
		void Deactivate();

	UFUNCTION(BlueprintImplementableEvent)
		void OnDeactivate();

	UFUNCTION(BlueprintPure)
		bool IsActive() const;

	UFUNCTION(BlueprintPure)
		ESkills GetSkillType() const;

	UFUNCTION(BlueprintCallable)
		void SetSkillType(ESkills Skill);

	UFUNCTION(BlueprintPure)
		float GetDuration() const;

	UFUNCTION(BlueprintCallable)
		virtual void OnAttackOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
