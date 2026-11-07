// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "MapButton.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMapButtonClicked, FVector);

UCLASS()
class FEBRICULA_API UMapButton : public UButton
{
	GENERATED_BODY()
public:
	FOnMapButtonClicked OnMapButtonClicked;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location;
	
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	
private:
	UFUNCTION()
	void HandleClicked();
};
