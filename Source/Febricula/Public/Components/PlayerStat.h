// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerStat.generated.h"

UENUM(BlueprintType)
enum class EPlayerStat : uint8
{
	Health,
	Stress,
	Int,
	Wis,
	Luck,
	Courage,
	HandWork,
	Speech,
	Max UMETA(Hidden)
};