// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy_AI.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

// Sets default values
AEnemy_AI::AEnemy_AI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

//void AEnemy_AI::LockOn()
//{
//}
//
//float AEnemy_AI::GetDistanceFromPlayer()
//{
//	ACharacter* PlayerCharacter = GetWorld()->GetFirstPlayerController()->GetCharacter();
//	float DistanceToCharacter = GetDistanceTo(PlayerCharacter);
//
//	return DistanceToCharacter;
//}
//
//float AEnemy_AI::GetDistanceFrom(AActor* OtherActor)
//{
//	float DistanceToActor = GetDistanceTo(OtherActor);
//
//	return DistanceToActor;
//}