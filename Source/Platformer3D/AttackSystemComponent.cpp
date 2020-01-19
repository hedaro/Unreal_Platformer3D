// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/CharacterMovementComponent.h"
#include "AttackSystemComponent.h"

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

	IsAttacking = true;
	SaveAttack = true;

	// There may be a better place to make these checks and a way to handle if any of them fails
	if (OwnerCharacterRef && AttacksArray.Num() > 0 && ComboCount < AttacksArray.Num() && AttacksArray[ComboCount].AnimMontage)
	{
		ComboCount = (ComboCount + 1) % AttacksArray.Num(); // Is it right to call before or after?????????????

		OwnerCharacterRef->PlayAnimMontage(AttacksArray[ComboCount].AnimMontage);
	}
}

void UAttackSystemComponent::SaveComboAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("COMBO SAVED"));
	SaveAttack = false;
}

void UAttackSystemComponent::ResetCombo()
{
	IsAttacking = false;
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
	ResetCombo();
	if (OwnerCharacterRef && AttacksArray.Num() > 0 && AttacksArray[ComboCount].AnimMontage)
	{
		OwnerCharacterRef->StopAnimMontage(AttacksArray[ComboCount].AnimMontage);
	}
}

