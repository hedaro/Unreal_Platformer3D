// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy_AI.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
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
void AEnemy_AI::OnSeePawn(APawn* OtherPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("I CAN SEE YOU!"));
	if (!SeenPlayer)
	{
		SeenPlayer = true;

	}
}