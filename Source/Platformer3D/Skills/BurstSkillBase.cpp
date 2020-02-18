// Fill out your copyright notice in the Description page of Project Settings.

//Engine libraries
#include "BurstSkillBase.h"
#include "Kismet/GameplayStatics.h"
//Custom libraries
#include "Interfaces/DamagableObject_Interface.h"


void ABurstSkillBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Burst begin play"));
	if (!Initialized)
	{
		Deactivate();
		Initialized = true;
	}
	
}

// Add default functionality here for any IBurstSkill_Interface functions that are not pure virtual.
void ABurstSkillBase::Activate()
{
	UE_LOG(LogTemp, Warning, TEXT("Burst activate"));
	Active = true;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	OnActivate();
}

void ABurstSkillBase::Deactivate()
{
	UE_LOG(LogTemp, Warning, TEXT("Burst deactivate"));
	Active = false;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	OnDeactivate();
}

bool ABurstSkillBase::IsActive() const
{
	return Active;
}

ESkills ABurstSkillBase::GetSkillType() const
{
	return SkillType;
}

void ABurstSkillBase::SetSkillType(ESkills Skill)
{
	SkillType = Skill;
}

float ABurstSkillBase::GetDuration() const
{
	return Duration;
}

void ABurstSkillBase::OnAttackOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("AttackOverlap"));
	if (OtherActor != this && OtherActor != GetOwner() && OtherActor->GetClass()->ImplementsInterface(UDamagableObject_Interface::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyOverlap"));
		IDamagableObject_Interface* DamagableObject = Cast<IDamagableObject_Interface>(OtherActor);
		if (DamagableObject && DamagableObject->GetIsAlive())
		{
			UE_LOG(LogTemp, Warning, TEXT("EnemyDamaged"));
			UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, NULL);

			DamagableObject->ReactToDamage(0.f);
		}
	}
}