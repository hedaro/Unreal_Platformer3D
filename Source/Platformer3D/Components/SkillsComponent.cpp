// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillsComponent.h"

// Sets default values for this component's properties
USkillsComponent::USkillsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void USkillsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
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

// Called every frame
//void USkillsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

