// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy_AI.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemy_AI::AEnemy_AI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CharacterMovementComponent = GetCharacterMovement();

	// Create Health Component
	HealthComponent = CreateDefaultSubobject<UHealthActorComponent>(TEXT("HEALTH"));
}

// Called when the game starts or when spawned
void AEnemy_AI::BeginPlay()
{
	Super::BeginPlay();
	
	//***** This is just a reminder to remember how to check if a given object implements an Interface *****//
	//***** Syntax is: MyObject->GetClass()->ImplementsInterface(MyInterface::StaticClass())  *****//
	//***** ¿¿¿MyInterface MUST be an interface class???  *****//
	//this->GetClass()->ImplementsInterface(AEnemy_AI::StaticClass());

	AIController = GetController();
}

// Called every frame
void AEnemy_AI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy_AI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

/** MY BEHAVIOUR **/

float AEnemy_AI::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (EventInstigator != AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Took %f damage"), Damage);
		HealthComponent->DecreaseHealth(Damage);
		ReactToDamage();
	}

	return Damage;
}

void AEnemy_AI::ReactToDamage()
{
	UE_LOG(LogTemp, Warning, TEXT("Am i alive? %d"), HealthComponent->IsAlive());
	UE_LOG(LogTemp, Warning, TEXT("Remaining health %f"), HealthComponent->GetCurrentHealth());
	if (HealthComponent->IsAlive())
	{
		if (DamageMontage)
		{
			UE_LOG(LogTemp, Warning, TEXT("Damage montage: %s"), *DamageMontage->GetName());
			PlayAnimMontage(DamageMontage);
		}
	}
	else
	{
		CharacterMovementComponent->DisableMovement();
		UE_LOG(LogTemp, Warning, TEXT("Death montage: %s"), *DeathMontage->GetName());
		if (DeathMontage)
		{
			PlayAnimMontage(DeathMontage);
		}
	}
}