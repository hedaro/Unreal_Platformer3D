// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "GameFramework/Character.h"
#include "Interfaces/DamagableObject_Interface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));

	ForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Forward Arrow"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	/*if (SphereCollision)
	{
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnProjectileOverlap);
	}
	
	GetWorldTimerManager().ClearTimer(ProjectileTimerHandle);
	GetWorldTimerManager().SetTimer(ProjectileTimerHandle, this, &AProjectile::Fire, 0.2f, false);*/

	if (SphereCollision)
	{
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnProjectileOverlap);
	}
	GetWorldTimerManager().ClearTimer(ProjectileTimerHandle);
	GetWorldTimerManager().SetTimer(ProjectileTimerHandle, this, &AProjectile::DestroyProjectile, LifeTime, false);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::SetDamage(float ProjectileDamage)
{
	Damage = ProjectileDamage;
}

void AProjectile::Fire()
{
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetWorldTimerManager().ClearTimer(ProjectileTimerHandle);
	GetWorldTimerManager().SetTimer(ProjectileTimerHandle, this, &AProjectile::DestroyProjectile, LifeTime, false);
}

void AProjectile::DestroyProjectile()
{
	GetWorldTimerManager().ClearTimer(ProjectileTimerHandle);
	Destroy();
}

void AProjectile::OnProjectileOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor != this && OtherActor != GetOwner() && OtherActor->GetClass()->ImplementsInterface(UDamagableObject_Interface::StaticClass()))
	{
		IDamagableObject_Interface* DamagableObject = Cast<IDamagableObject_Interface>(OtherActor);
		if (DamagableObject && DamagableObject->GetIsAlive())
		{
			AController* OwnerController = NULL;
			ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
			if (OwnerCharacter)
			{
				OwnerController = OwnerCharacter->GetController();
			}

			float AppliedDamage = UKismetMathLibrary::RandomFloatInRange(Damage, Damage);
			UGameplayStatics::ApplyDamage(OtherActor, AppliedDamage, OwnerController, this, NULL);
			UE_LOG(LogTemp, Warning, TEXT("Hit %s for %f damage"), *OtherActor->GetName(), AppliedDamage);
			DamagableObject->ReactToDamage(0.f);
		}

		DestroyProjectile();
	}
}