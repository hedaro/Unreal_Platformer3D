// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"

#include "AttackSystemComponent.generated.h"


USTRUCT(BlueprintType)
struct FAttack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		UAnimMontage* AnimMontage;

	UPROPERTY(EditAnywhere)
		float Damage;

	UPROPERTY(EditAnywhere)
		float LaunchForce;

	UPROPERTY(EditAnywhere)
		float JumpForce;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PLATFORMER3D_API UAttackSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttackSystemComponent();

private:
	ACharacter* OwnerCharacterRef;

	/*** Attack ***/
	bool bIsAttacking = false;
	bool SaveAttack = false;
	int ComboCount = 0;
	float AttackCooldownTimer;
	FTimerHandle AttackTimerHandle;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		TArray<FAttack> AttacksArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		float AttackCooldown = 0.f;

	UFUNCTION()
		bool CanAttack();

	UFUNCTION()
		void NextAttack();

	UFUNCTION(BlueprintCallable)
		void SaveComboAttack();

	UFUNCTION(BlueprintCallable)
		void ResetCombo();

	UFUNCTION(BlueprintCallable)
		void ApplyAttackLaunch();

	UFUNCTION()
		void EndAttackLaunch();

	UFUNCTION()
		void CancelAttack();

	UFUNCTION()
		bool IsAttacking();
		
	UFUNCTION()
		bool IsAttackAnimation();

	UFUNCTION()
		int GetMaxCombo();

	UFUNCTION()
		int GetCurrentCombo();

	UFUNCTION()
		float GetAttackDamage();
};
