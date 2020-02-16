// Fill out your copyright notice in the Description page of Project Settings.


#include "BurstSkill_Interface.h"

// Add default functionality here for any IBurstSkill_Interface functions that are not pure virtual.
void IBurstSkill_Interface::Activate_Implementation()
{
	Active = true;
}

void IBurstSkill_Interface::Deactivate_Implementation()
{
	Active = false;
}

bool IBurstSkill_Interface::IsActive_Implementation() const
{
	return Active;
}

ESkills IBurstSkill_Interface::GetSkillType_Implementation() const
{
	return SkillType;
}