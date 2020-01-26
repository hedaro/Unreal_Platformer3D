// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackSystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UAttackSystemComponent::UAttackSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAttackSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// This component is meant for ACharacters
	OwnerCharacterRef = Cast<ACharacter>(GetOwner());
}

// Called every frame
void UAttackSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Ugly and hacky way to simulate a cooldown timer
	if (AttackCooldownTimer > 0.f)
	{
		AttackCooldownTimer -= DeltaTime;
	}
}

bool UAttackSystemComponent::CanAttack()
{
	return !SaveAttack && AttackCooldownTimer <= 0.f;
}

void UAttackSystemComponent::NormalAttack()
{
	// Ugly and hacky way to simulate a cooldown timer, check Tick() function for update
	AttackCooldownTimer = AttackCooldown;

	bIsAttacking = true;
	SaveAttack = true;

	// There may be a better place to make these checks and a way to handle if any of them fails
	if (OwnerCharacterRef && NormalAttacksArray.Num() > 0 && NormalComboCount < NormalAttacksArray.Num() && NormalAttacksArray[NormalComboCount].AnimMontage)
	{
		CurrentComboType = NormalAttacksArray[NormalComboCount].AttackType;
		OwnerCharacterRef->PlayAnimMontage(NormalAttacksArray[NormalComboCount].AnimMontage);
		// NormalComboCount = (NormalComboCount + 1) % NormalAttacksArray.Num(); // Is it right to call before or after?????????????
	}
}

void UAttackSystemComponent::HeavyAttack()
{
	// Ugly and hacky way to simulate a cooldown timer, check Tick() function for update
	AttackCooldownTimer = AttackCooldown;

	bIsAttacking = true;
	SaveAttack = true;

	// There may be a better place to make these checks and a way to handle if any of them fails
	if (OwnerCharacterRef && HeavyAttacksArray.Num() > 0 && HeavyComboCount < HeavyAttacksArray.Num() && HeavyAttacksArray[HeavyComboCount].AnimMontage)
	{
		CurrentComboType = HeavyAttacksArray[HeavyComboCount].AttackType;
		OwnerCharacterRef->PlayAnimMontage(HeavyAttacksArray[HeavyComboCount].AnimMontage);
	}
}

void UAttackSystemComponent::AerialAttack()
{
	// Ugly and hacky way to simulate a cooldown timer, check Tick() function for update
	AttackCooldownTimer = AttackCooldown;

	bIsAttacking = true;
	SaveAttack = true;

	// There may be a better place to make these checks and a way to handle if any of them fails
	if (OwnerCharacterRef && AerialAttacksArray.Num() > 0 && AerialComboCount < AerialAttacksArray.Num() && AerialAttacksArray[AerialComboCount].AnimMontage)
	{
		CurrentComboType = AerialAttacksArray[AerialComboCount].AttackType;
		OwnerCharacterRef->PlayAnimMontage(AerialAttacksArray[AerialComboCount].AnimMontage);
	}
}

void UAttackSystemComponent::SaveComboAttack()
{
	SaveAttack = false;
	switch (CurrentComboType)
	{
	case EAttackType::AT_NormalAttack:
		NormalComboCount = (NormalComboCount + 1) % NormalAttacksArray.Num();
		break;
	case EAttackType::AT_HeavyAttack:
		HeavyComboCount = (HeavyComboCount + 1) % HeavyAttacksArray.Num();
		break;
	case EAttackType::AT_AerialAttack:
		AerialComboCount = (AerialComboCount + 1) % AerialAttacksArray.Num();
		break;
	}
}

void UAttackSystemComponent::ResetCombo()
{
	bIsAttacking = false;
	SaveAttack = false;
	/***** Test if when we should keep some combos *****/
	NormalComboCount = 0;
	HeavyComboCount = 0;
	AerialComboCount = 0;
}

void UAttackSystemComponent::CancelAttack()
{
	if (OwnerCharacterRef && NormalAttacksArray.Num() > 0)
	{
		int CurrentAttack = GetCurrentCombo();
		if (NormalAttacksArray[CurrentAttack].AnimMontage)
		{
			OwnerCharacterRef->StopAnimMontage(NormalAttacksArray[CurrentAttack].AnimMontage);
		}
	}

	ResetCombo();
}

bool UAttackSystemComponent::IsAttacking()
{
	return bIsAttacking;
}

bool UAttackSystemComponent::IsAttackAnimation()
{
	return SaveAttack;
}


int UAttackSystemComponent::GetMaxCombo()
{
	return NormalAttacksArray.Num();
}

int UAttackSystemComponent::GetCurrentCombo()
{
	FAttack CurrentAttack;
	int ComboCount;
	int ComboCountSize;

	switch (CurrentComboType)
	{
	case EAttackType::AT_NormalAttack:
		CurrentAttack = NormalAttacksArray[NormalComboCount];
		ComboCountSize = NormalAttacksArray.Num();
		ComboCount = NormalComboCount;
		break;
	case EAttackType::AT_HeavyAttack:
		CurrentAttack = HeavyAttacksArray[HeavyComboCount];
		ComboCountSize = HeavyAttacksArray.Num();
		ComboCount = HeavyComboCount;
		break;
	case EAttackType::AT_AerialAttack:
		CurrentAttack = AerialAttacksArray[AerialComboCount];
		ComboCountSize = AerialAttacksArray.Num();
		ComboCount = AerialComboCount;
		break;
	}

	return SaveAttack ? ComboCount : bIsAttacking ? (ComboCount + ComboCountSize - 1) % ComboCountSize : 0;
}

FAttack UAttackSystemComponent::GetCurrentAttack()
{
	FAttack CurrentAttack = FAttack();
	if (!bIsAttacking)
	{
		return CurrentAttack;
	}

	int ComboCount = GetCurrentCombo();

	switch (CurrentComboType)
	{
	case EAttackType::AT_NormalAttack:
		if (NormalAttacksArray.Num() > 0 && ComboCount < NormalAttacksArray.Num())
		{
			CurrentAttack = NormalAttacksArray[ComboCount];
		}
		break;
	case EAttackType::AT_HeavyAttack:
		if (HeavyAttacksArray.Num() > 0 && ComboCount < HeavyAttacksArray.Num())
		{
			CurrentAttack = HeavyAttacksArray[ComboCount];
		}
		break;
	case EAttackType::AT_AerialAttack:
		if (AerialAttacksArray.Num() > 0 && ComboCount < AerialAttacksArray.Num())
		{
			CurrentAttack = AerialAttacksArray[ComboCount];
		}
		break;
	}

	return CurrentAttack;
}

EAttackType UAttackSystemComponent::GetCurrentAttackType()
{
	return GetCurrentAttack().AttackType;
}

float UAttackSystemComponent::GetAttackDamage()
{
	return GetCurrentAttack().Damage;
}