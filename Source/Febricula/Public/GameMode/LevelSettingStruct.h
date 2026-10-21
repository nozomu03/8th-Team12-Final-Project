#pragma once
#include "LevelSettingStruct.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FLevelSettingStruct
{
	GENERATED_BODY();
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> Level;
};
