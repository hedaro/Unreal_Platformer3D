// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Engine libraries
#include "CoreMinimal.h"
#include "UObject/Interface.h"
// Custom libraries
#include "Enums/Skills.h"

#include "BurstSkill_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UBurstSkill_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PLATFORMER3D_API IBurstSkill_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
private:
	bool Active;
	ESkills SkillType;

public:
	UFUNCTION(BlueprintNativeEvent)
		void Activate();

	virtual void Activate_Implementation();

	UFUNCTION(BlueprintNativeEvent)
		void Deactivate();

	virtual void Deactivate_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		bool IsActive() const;

	virtual bool IsActive_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		ESkills GetSkillType() const;

	virtual ESkills GetSkillType_Implementation() const;
};
