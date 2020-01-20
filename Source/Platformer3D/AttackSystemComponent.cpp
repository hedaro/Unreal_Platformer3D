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

void UAttackSystemComponent::NextAttack()
{
	// Ugly and hacky way to simulate a cooldown timer, check Tick() function for update
	AttackCooldownTimer = AttackCooldown;

	bIsAttacking = true;
	SaveAttack = true;

	// There may be a better place to make these checks and a way to handle if any of them fails
	if (OwnerCharacterRef && AttacksArray.Num() > 0 && ComboCount < AttacksArray.Num() && AttacksArray[ComboCount].AnimMontage)
	{
		OwnerCharacterRef->PlayAnimMontage(AttacksArray[ComboCount].AnimMontage);

		// ComboCount = (ComboCount + 1) % AttacksArray.Num(); // Is it right to call before or after?????????????
	}
}

void UAttackSystemComponent::SaveComboAttack()
{
	SaveAttack = false;
	ComboCount = (ComboCount + 1) % AttacksArray.Num();
}

void UAttackSystemComponent::ResetCombo()
{
	bIsAttacking = false;
	SaveAttack = false;
	ComboCount = 0;
}

void UAttackSystemComponent::ApplyAttackLaunch()
{
	if (OwnerCharacterRef && AttacksArray.Num() > 0 && ComboCount < AttacksArray.Num())
	{
		FVector AttackDirection =
			OwnerCharacterRef->GetActorForwardVector() * AttacksArray[ComboCount].LaunchForce
			+ OwnerCharacterRef->GetActorUpVector() * AttacksArray[ComboCount].JumpForce;

		OwnerCharacterRef->GetCharacterMovement()->GroundFriction = 0.f;
		OwnerCharacterRef->GetCharacterMovement()->Velocity = AttackDirection;
		OwnerCharacterRef->GetWorldTimerManager().ClearTimer(AttackTimerHandle);
		OwnerCharacterRef->GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &UAttackSystemComponent::EndAttackLaunch, 0.25f, false);
	}
}

void UAttackSystemComponent::EndAttackLaunch()
{
	if (OwnerCharacterRef)
	{
		OwnerCharacterRef->GetCharacterMovement()->GroundFriction = 8.f;
	}
}

void UAttackSystemComponent::CancelAttack()
{
	if (OwnerCharacterRef && AttacksArray.Num() > 0)
	{
		int CurrentAttack = GetCurrentCombo();
		if (AttacksArray[CurrentAttack].AnimMontage)
		{
			OwnerCharacterRef->StopAnimMontage(AttacksArray[CurrentAttack].AnimMontage);
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
	return AttacksArray.Num();
}

int UAttackSystemComponent::GetCurrentCombo()
{
	return SaveAttack ? ComboCount : bIsAttacking ? (ComboCount + AttacksArray.Num() - 1) % AttacksArray.Num() : 0;
}

float UAttackSystemComponent::GetAttackDamage()
{
	if (bIsAttacking && AttacksArray.Num() > 0)
	{
		int CurrentAttack = GetCurrentCombo();
		return AttacksArray[CurrentAttack].Damage;
	}

	return 0.f;
}