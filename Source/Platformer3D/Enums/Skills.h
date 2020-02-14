#pragma once

UENUM(BlueprintType)
enum class ESkills : uint8
{
	ES_None UMETA(DisplayName = "None"),
	ES_Dash UMETA(DisplayName = "Dash"),
	ES_HeavyAttack UMETA(DisplayName = "Heavy Attack"),
	ES_SpinningBlades UMETA(DisplayName = "Spining Blades"),
	ES_SummonDragon UMETA(DisplayName = "Summon Dragon"),
	ES_ShadowClones UMETA(DisplayName = "Shadow Clones")
};

