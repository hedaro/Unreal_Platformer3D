// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillsComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"

// Sets default values for this component's properties
USkillsComponent::USkillsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void USkillsComponent::PostLoad()
{
	Super::PostLoad();

	CharacterOwner = Cast<ACharacter>(GetOwner());
}

// Called when the game starts
void USkillsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	for (auto& S : Skills)
	{
		if (S.Value.SkillToSpawn)
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = CharacterOwner;
			SpawnParameters.Instigator = CharacterOwner;
			S.Value.SkillInstance = GetWorld()->SpawnActor<ABurstSkillBase>(S.Value.SkillToSpawn, CharacterOwner->GetActorLocation(), CharacterOwner->GetActorRotation(), SpawnParameters);
			if (S.Value.SkillInstance)
			{
				FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, false);
				S.Value.SkillInstance->AttachToComponent(CharacterOwner->GetCapsuleComponent(), TransformRules);
				S.Value.SkillInstance->SetSkillType(S.Key);
			}
		}
	}
}

bool USkillsComponent::FindSkill(ESkills Skill) const
{
	return Skills.Contains(Skill);
}

bool USkillsComponent::IsSkillAcquired(ESkills Skill) const
{
	if (Skills.Contains(Skill))
	{
		return Skills[Skill].Acquired;
	}
	return false;
}

TMap<ESkills, FSkill> USkillsComponent::GetSkillsData() const
{
	return Skills;
}

void USkillsComponent::SetSkillsData(TMap<ESkills, FSkill> SkillsData)
{
	Skills = SkillsData;
}

FSkill USkillsComponent::GetSkill(ESkills Skill) const
{
	const FSkill* SkillData = Skills.Find(Skill);

	if (SkillData)
	{
		return *SkillData;
	}

	return FSkill{ ESkills::ES_None, 0, true };
}

bool USkillsComponent::IsSkillAvailable(ESkills Skill) const
{
	const FSkill* SkillData = Skills.Find(Skill);

	if (!SkillData)
	{
		return false;
	}
	else if (SkillData->SkillRequired != ESkills::ES_None)
	{
		const FSkill* SkillRequired = Skills.Find(SkillData->SkillRequired);
		if (SkillRequired)
		{
			return SkillRequired->Acquired && !SkillData->Acquired;
		}
	}

	return !SkillData->Acquired;
}

void USkillsComponent::AcquireSkill(ESkills Skill)
{
	if (Skills.Contains(Skill) && IsSkillAvailable(Skill))
	{
		Skills[Skill].Acquired = true;
	}
}

void USkillsComponent::BurstSkill(ESkills Skill)
{
	if (Skills.Contains(Skill) && Skills[Skill].SkillToSpawn && Skills[Skill].SkillInstance && !Skills[Skill].SkillInstance->IsActive())
	{
		ActiveBurstSkill = Skills[Skill].SkillInstance;
		ActiveBurstSkill->Activate();
		GetWorld()->GetTimerManager().SetTimer(SkillTimerHandle, this, &USkillsComponent::CancelBurstSkill, ActiveBurstSkill->GetDuration(), false);
	}
}

void USkillsComponent::CancelBurstSkill()
{
	ActiveBurstSkill->Deactivate();
	ActiveBurstSkill = NULL;
}

bool USkillsComponent::IsBurstSkillActive() const
{
	return ActiveBurstSkill && ActiveBurstSkill->IsActive();
}

// Called every frame
//void USkillsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

