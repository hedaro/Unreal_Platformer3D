// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"

#include "LockOn_Interface.generated.h"

/**
 * 
 */
UINTERFACE(Blueprintable)
class PLATFORMER3D_API ULockOn_Interface : public UInterface
{
	GENERATED_BODY()

};

class ILockOn_Interface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "LockOn")
	void LockOn();

	/*UFUNCTION(Category = "LockOn")
	virtual float GetDistanceFromPlayer() = 0;

	UFUNCTION(Category = "LockOn")
	virtual float GetDistanceFrom(AActor* OtherActor) = 0;*/
};