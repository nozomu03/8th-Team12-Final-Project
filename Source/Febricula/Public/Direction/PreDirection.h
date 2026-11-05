#pragma once
#include  "PreDirection.generated.h"

UENUM(Blueprintable, BlueprintType)
enum class EPreDirection : uint8
{
	None,
	InitialFadeOut,
	InitialFadeIn,
	DeinitialFadeOut,
};

USTRUCT(Blueprintable, BlueprintType)
struct FPreDirectionBlock
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPreDirection PreDirection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> ConnectedLevel;
	
	
	FPreDirectionBlock()
	{
		PreDirection = EPreDirection::InitialFadeOut;
		ConnectedLevel = nullptr;
	}
	
	explicit FPreDirectionBlock(EPreDirection NowPreDirection)
	{
		PreDirection = NowPreDirection;
		ConnectedLevel = nullptr;
	}
	
	explicit FPreDirectionBlock(EPreDirection NowPreDirection, TSoftObjectPtr<UWorld> NowConnectedLevel)
	{
		PreDirection = NowPreDirection;
		ConnectedLevel = NowConnectedLevel;
	}
};
