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

	// Setting MinMax Distance to player
	DistanceToPlayerMinMax = FVector2D(200.f, 1000.f);

	// Create Pawn Sensing Component
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensor"));
	PawnSensingComponent->SetPeripheralVisionAngle(90.f);
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

	PawnSensingComponent->SightRadius = DistanceToPlayerMinMax.Y;
	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	Cast<AAIController>(Controller)->bSetControlRotationFromPawnOrientation = 0;
}

// Called every frame
void AEnemy_AI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HealthComponent->IsAlive() && SeenPlayer)
	{
		SeekPlayer();
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
	if (!SeenPlayer && OtherPawn == PlayerCharacter && HealthComponent->IsAlive())
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
	if (HealthComponent->IsAlive() && !AttackSystem->IsAttackAnimation() && !Flinch)
	{
		//if (HealthComponent->GetCurrentHealth() <= 50.f)
		//{
		//	// Make some low health actions
		//}
		//else 
		if (DistanceToPlayer() >= DistanceToPlayerMinMax.X)
		{
			//Cast<AAIController>(Controller)->MoveToActor(PlayerCharacter, 5.f);
			MoveForward(1.0);
		}
		else
		{
			StartAttack();
		}
		
		if (DistanceToPlayer() > DistanceToPlayerMinMax.Y)
		{
			GetCharacterMovement()->bOrientRotationToMovement = true;
			bUseControllerRotationYaw = false;
			LockOffTarget();
			SeenPlayer = false;
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

	// Why does it stop when getting hit and why it doesnt cause damage to player (Functions are actually base class)
	if (!HealthComponent->IsAlive())
	{
		SeenPlayer = false;
	}
}

float AEnemy_AI::DistanceToPlayer()
{
	return GetDistanceTo(PlayerCharacter);
}