// Fill out your copyright notice in the Description page of Project Settings.


#include "Cache/ActorCacheSubsystem.h"

#include "Engine/TargetPoint.h"
#include "GameMode/TargetTypeComponent.h"

void UActorCacheSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
}

FTransform UActorCacheSubsystem::GetTargetTransform(FName TargetTag, ETargetTypeRole TargetTypeRole)
{
	if (AActor* NowTarget = CachedActors.FindRef(TargetTag).Get())
	{
		if (UTargetTypeComponent* TargetTypeComponent = NowTarget->GetComponentByClass<UTargetTypeComponent>())
		{
			if (TargetTypeComponent->TargetTypeRole == TargetTypeRole || TargetTypeRole == ETargetTypeRole::WildCard)
			{
				return NowTarget->GetTransform();
			}
			UE_LOG(LogTemp, Warning, TEXT("%s: TargetPoint와 목표 타입이 일치하지 않음"), *TargetTag.ToString())
			return FTransform();
		}
		UE_LOG(LogTemp, Warning, TEXT("%s: TargetPoint에 UTargetTypeComponent가 없음"), *TargetTag.ToString())
		return FTransform();
	}
	UE_LOG(LogTemp, Warning, TEXT("%s: TargetPoint를 찾을 수 없음"), *TargetTag.ToString())
	return FTransform();
}

FVector UActorCacheSubsystem::GetTargetLocation(FName TargetTag, ETargetTypeRole TargetTypeRole)
{
	if (AActor* NowTarget = CachedActors.FindRef(TargetTag).Get())
	{
		if (UTargetTypeComponent* TargetTypeComponent = NowTarget->GetComponentByClass<UTargetTypeComponent>())
		{
			if (TargetTypeComponent->TargetTypeRole == TargetTypeRole || TargetTypeRole == ETargetTypeRole::WildCard)
			{
				return NowTarget->GetActorLocation();
			}
			UE_LOG(LogTemp, Warning, TEXT("%s: TargetPoint와 목표 타입이 일치하지 않음"), *TargetTag.ToString())
			return FVector::Zero();
		}
		UE_LOG(LogTemp, Warning, TEXT("%s: TargetPoint에 UTargetTypeComponent가 없음"), *TargetTag.ToString())
		return FVector::Zero();
	}
	UE_LOG(LogTemp, Warning, TEXT("%s: TargetPoint를 찾을 수 없음"), *TargetTag.ToString())
	return FVector::Zero();
}

TArray<FDisplayPoint> UActorCacheSubsystem::GetTargetLocationByRole(ETargetTypeRole TargetTypeRole)
{
	TArray<FDisplayPoint> Locations;

	for (const TPair<FName, TWeakObjectPtr<AActor>>& Pair : CachedActors)
	{
		AActor* Target = Pair.Value.Get();

		if (!IsValid(Target))
		{
			continue;
		}

		UTargetTypeComponent* Component =
			Target->GetComponentByClass<UTargetTypeComponent>();

		if (!Component)
		{
			continue;
		}

		if (TargetTypeRole != ETargetTypeRole::WildCard &&
			Component->TargetTypeRole != TargetTypeRole)
		{
			continue;
		}

		Locations.Add(FDisplayPoint(Pair.Key, Target->GetActorLocation()));
	}

	return Locations;
}

// [skydodo] 2026-10-13 버그 수정: 태그가 없을 때 경고 후 '존재하지 않는 액터' 경고까지 이어서 찍던 것 수정
bool UActorCacheSubsystem::AddActorToCache(AActor* Actor)
{
	if (Actor)
	{
		if (Actor->Tags.Num() > 0)
		{
			CachedActors.Add(Actor->Tags[0], Actor);
			return true;
		}
		UE_LOG(LogTemp, Warning, TEXT("액터에 태그가 할당되지 않음"));
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("존재하지 않는 액터"));
	return false;
}

bool UActorCacheSubsystem::RemoveActorFromCache(FName TargetTag)
{
	if (CachedActors.Contains(TargetTag))
	{
		CachedActors.Remove(TargetTag);
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("캐싱되지 않은 Target Tag"))
	return false;
}

UTargetTypeComponent* UActorCacheSubsystem::GetTargetTypeComponent(FName TargetTag)
{
	if (AActor* Target = CachedActors.FindRef(TargetTag).Get())
	{
		if (UTargetTypeComponent* TargetTypeComponent = Target->GetComponentByClass<UTargetTypeComponent>())
		{
			return TargetTypeComponent;
		}
	}
	return nullptr;
}

// [skydodo] 2026-10-13 버그 수정: 헤더만 Shipping 가드가 있고 정의는 없어 Shipping 빌드가 깨졌다. 캐시가 약참조로 바뀌어 파괴된 액터도 확인
#if !UE_BUILD_SHIPPING
void UActorCacheSubsystem::PrintAllCachedTarget() const
{
	UE_LOG(LogTemp, Warning, TEXT("PrintAllCachedTarget"));
	for (const TPair<FName, TWeakObjectPtr<AActor>>& Pair : CachedActors)
	{
		if (const AActor* Target = Pair.Value.Get())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s : %s : %s"), *Pair.Key.ToString(), *Target->GetActorLocation().ToString(), *Target->GetActorRotation().ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s : (파괴됨)"), *Pair.Key.ToString());
		}
	}
}
#endif
