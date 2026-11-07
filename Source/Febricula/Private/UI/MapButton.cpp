// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MapButton.h"

TSharedRef<SWidget> UMapButton::RebuildWidget()
{
	OnClicked.RemoveDynamic(this, &UMapButton::HandleClicked);
	OnClicked.AddDynamic(this, &UMapButton::HandleClicked);
	return Super::RebuildWidget();
}

void UMapButton::HandleClicked()
{
	OnMapButtonClicked.Broadcast(Location);
}



