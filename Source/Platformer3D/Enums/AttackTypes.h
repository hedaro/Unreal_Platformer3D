// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	AT_NormalAttack UMETA(DisplayName="Normal Attack"),
	AT_HeavyAttack UMETA(DisplayName = "Heavy Attack"),
	AT_AerialAttack UMETA(DisplayName = "Aerial Attack")
};
