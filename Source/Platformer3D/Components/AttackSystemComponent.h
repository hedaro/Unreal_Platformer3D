// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "Enums/AttackTypes.h"

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
		float AdvanceForce;

	UPROPERTY(EditAnywhere)
		float JumpForce;

	UPROPERTY(EditAnywhere)
		EAttackType AttackType;
};

USTRUCT(BlueprintType)
struct FRangedAttack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		UAnimMontage* StartAnimMontage;

	UPROPERTY(EditAnywhere)
		UAnimMontage* AimAnimMontage;

	UPROPERTY(EditAnywhere)
		UAnimMontage* FireAnimMontage;

	UPROPERTY(EditAnywhere)
		AActor* Projectile;

	UPROPERTY(EditAnywhere)
		float Damage;

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
	bool bIsAiming = false;
	int NormalComboCount = 0;
	int HeavyComboCount = 0;
	int AerialComboCount = 0;
	float AttackCooldownTimer;
	FTimerHandle AttackTimerHandle;
	EAttackType CurrentComboType;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		TArray<FAttack> NormalAttacksArray;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		TArray<FAttack> HeavyAttacksArray;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		TArray<FAttack> AerialAttacksArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		FRangedAttack RangedAttackStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
		float AttackCooldown = 0.f;

	UFUNCTION()
		bool CanAttack();

	UFUNCTION()
		void NormalAttack();

	UFUNCTION()
		void HeavyAttack();

	UFUNCTION()
		void AerialAttack();

	UFUNCTION(BlueprintCallable)
		void SaveComboAttack();

	UFUNCTION(BlueprintCallable)
		void ResetCombo();

	UFUNCTION()
		void CancelAttack();

	UFUNCTION()
		void RangedAttack();

	UFUNCTION()
		void FireRangedAttack();

	UFUNCTION()
		void CancelRangedAttack();

	UFUNCTION()
		bool IsAttacking();

	UFUNCTION()
		bool IsAiming();
		
	UFUNCTION()
		bool IsAttackAnimation();

	UFUNCTION()
		int GetMaxCombo();

	UFUNCTION()
		int GetCurrentCombo();

	UFUNCTION()
		FAttack GetCurrentAttack();

	UFUNCTION()
		EAttackType GetCurrentAttackType();

	UFUNCTION()
		float GetAttackDamage();
};
