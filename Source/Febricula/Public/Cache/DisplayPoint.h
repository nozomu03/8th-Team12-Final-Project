#pragma once
#include "DisplayPoint.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FDisplayPoint
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName PointName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector Location;
};
