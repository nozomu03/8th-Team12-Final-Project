// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ProgressSettings.generated.h"

/**
 * 
 */
UCLASS(Config="Game")
class FEBRICULA_API UProgressSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Config, BlueprintReadOnly)
	TSoftClassPtr<UUserWidget> PauseWidgetClass;
	
	UPROPERTY(EditAnywhere, Config, BlueprintReadOnly)
	TSoftClassPtr<UUserWidget> PreDirectionWidgetClass;
	
	UPROPERTY(EditAnywhere, Config, BlueprintReadOnly)
	TSoftObjectPtr<UInputMappingContext> DefaultInputMappingContext;
};
