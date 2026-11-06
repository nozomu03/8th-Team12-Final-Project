// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Cheat/FebriculaCheatManager.h"
#include "GameFramework/PlayerController.h"
#include "FebriculaPlayerController.generated.h"

class USunControllerComponent;
class AFebriculaCameraManager;
class UInputAction;
class UInputMappingContext;
class UUserWidget;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class AFebriculaPlayerController : public APlayerController
{
	GENERATED_BODY()
	AFebriculaPlayerController();
	
public:
	UFUNCTION(BlueprintCallable, Category = "Input | OpenedWidget")
	void AddWidget(UUserWidget* Widget);
	UFUNCTION(BlueprintCallable, Category = "Input | OpenedWidget")
	int32 GetWidgetCount();

	UFUNCTION(BlueprintCallable, Category = "Input | OpenedWidget")
	bool RemoveWidget();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input | Default Widget")
	TObjectPtr<UUserWidget> DefaultWidget;
	
	UPROPERTY(EditAnywhere, Category = "Input | Input Action")
	UInputAction* UIAction;
	
	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> UIMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	void HandleRemove(const FInputActionValue& InputActionValue);
	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input | OpenedWidget")
	TArray<UUserWidget*> OpenedWidgets;

	
};
