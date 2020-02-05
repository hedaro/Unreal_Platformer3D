// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum class EItem_Types : uint8
{
	IT_None UMETA(DisplayName = "None"),
	IT_HealthRecovery UMETA(DisplayName = "Health Recovery"),
	IT_Arrow UMETA(DisplayName = "Arrow"),
	IT_Key UMETA(DisplayName = "Key")
};
