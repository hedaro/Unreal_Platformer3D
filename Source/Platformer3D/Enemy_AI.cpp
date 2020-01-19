// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy_AI.h"

#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Components/ShapeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy_AI::AEnemy_AI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Pawn Sensing Component
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensor"));
	PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemy_AI::OnSeePawn);
}

// Called when the game starts or when spawned
void AEnemy_AI::BeginPlay()
{
	Super::BeginPlay();
	
	//***** This is just a reminder to remember how to check if a given object implements an Interface *****//
	//***** Syntax is: MyObject->GetClass()->ImplementsInterface(MyInterface::StaticClass())  *****//
	//***** ¿¿¿MyInterface MUST be an interface class???  *****//
	//this->GetClass()->ImplementsInterface(AEnemy_AI::StaticClass());

	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	Cast<AAIController>(Controller)->bSetControlRotationFromPawnOrientation = 0;
}

// Called every frame
void AEnemy_AI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SeenPlayer)
	{
		MoveForward(1.f);
	}
}

// Called to bind functionality to input
void AEnemy_AI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

/** MY BEHAVIOUR **/
void AEnemy_AI::OnSeePawn(APawn* OtherPawn)
{
	if (OtherPawn == PlayerCharacter && HealthComponent->IsAlive())
	{
		SeenPlayer = true;
		if (!NearestTarget)
		{
			GetCharacterMovement()->bOrientRotationToMovement = false;
			bUseControllerRotationYaw = true;
			LockOnTarget(OtherPawn);
		}
	}
}

void AEnemy_AI::SeekPlayer()
{
	if (HealthComponent->IsAlive() && !Flinch)
	{
		if (IsAttacking)
		{
			GetCharacterMovement()->StopMovementImmediately();
		}
		else
		{
			MoveForward(1.f);
		}
	}
}

void AEnemy_AI::DoDamage(AActor* Target)
{
	if (Target == PlayerCharacter)
	{
		UGameplayStatics::ApplyDamage(Target, BaseDamage, Controller, this, NULL);
	}
}

void AEnemy_AI::ReactToDamage()
{
	Super::ReactToDamage();

	LockOffTarget();
	SeenPlayer = false;
}