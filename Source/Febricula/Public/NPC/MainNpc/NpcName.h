#pragma once
#include "NpcName.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FNpcName : public FTableRowBase
{
	GENERATED_BODY();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
	FName NpcId;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
	FText NpcName;
};
