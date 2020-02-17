// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Engine libraries
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "Engine/Public/TimerManager.h"
// Custom libraries
#include "Enums/Skills.h"
#include "Skills/BurstSkillBase.h"

#include "SkillsComponent.generated.h"


USTRUCT(BlueprintType)
struct FSkill
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere)
		ESkills SkillRequired;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int Cost;

	UPROPERTY(VisibleAnywhere)
		bool Acquired;

	UPROPERTY(EditAnywhere)
		float LimitGaugeCost = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<ABurstSkillBase> SkillToSpawn;

	UPROPERTY()
		ABurstSkillBase* SkillInstance;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLATFORMER3D_API USkillsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USkillsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Skills")
		TMap<ESkills, FSkill> Skills;

	FTimerHandle SkillTimerHandle;

	ABurstSkillBase* ActiveBurstSkill;

	ACharacter* CharacterOwner;

public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void PostLoad() override;
	
	UFUNCTION(BlueprintPure)
		bool FindSkill(ESkills Skill) const;

	UFUNCTION(BlueprintPure)
		bool IsSkillAcquired(ESkills Skill) const;

	UFUNCTION(BlueprintPure)
		TMap<ESkills, FSkill> GetSkillsData() const;

	UFUNCTION(BlueprintCallable)
		void SetSkillsData(TMap<ESkills, FSkill> SkillsData);

	UFUNCTION(BlueprintPure)
		FSkill GetSkill(ESkills Skill) const;

	UFUNCTION(BlueprintPure)
		bool IsSkillAvailable(ESkills Skill) const;

	UFUNCTION(BlueprintCallable)
		void AcquireSkill(ESkills Skill);

	UFUNCTION(BlueprintCallable)
		void BurstSkill(ESkills Skill);

	UFUNCTION(BlueprintCallable)
		void CancelBurstSkill();

	UFUNCTION(BlueprintPure)
		bool IsBurstSkillActive() const;
};
