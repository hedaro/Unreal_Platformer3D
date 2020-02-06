// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

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
	
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWorldTimerManager().ClearTimer(ProjectileTimerHandle);
	GetWorldTimerManager().SetTimer(ProjectileTimerHandle, this, &AProjectile::Fire, 0.2f, false);
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
	GetWorldTimerManager().SetTimer(ProjectileTimerHandle, this, &AProjectile::DestroyProjectile, 0.2f, false);
}

void AProjectile::DestroyProjectile()
{
	Destroy();
}

void AProjectile::OnProjectileOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit %s"), *OtherActor->GetName());
	DestroyProjectile();
}