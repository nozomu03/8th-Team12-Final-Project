// Copyright Epic Games, Inc. All Rights Reserved.


#include "FebriculaPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Febricula.h"
#include "FebriculaCharacter.h"
#include "Dialogue/DialogueSubsystem.h"
#include "GameMode/LevelSetting.h"
#include "Interaction/InteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Light/SunControllerComponent.h"
#include "Progress/GameProgressSubsystem.h"
#include "UI/PlayerHUDWidget.h"
#include "Widgets/Input/SVirtualJoystick.h"

AFebriculaPlayerController::AFebriculaPlayerController()
{
	CheatClass = UFebriculaCheatManager::StaticClass();
}

void AFebriculaPlayerController::AddWidget(UUserWidget* Widget)
{
    if (!Widget) return;

    // 1. 위젯을 추가하기 전에 '이전 상태가 비어있었는지' 미리 체크
    bool bWasEmpty = OpenedWidgets.IsEmpty();

    // 2. 스택에 위젯 추가
    OpenedWidgets.Add(Widget);
	
	Widget->AddToViewport(10000);
	Widget->SetVisibility(ESlateVisibility::Visible);

    // 3. 0개 -> 1개가 되는 최초의 순간에만 IMC 교체 및 입력 모드 변경
    if (bWasEmpty)
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
        	InputSubsystem->ClearAllMappings();
        	
            for (UInputMappingContext* UI : UIMappingContexts)
            {
                InputSubsystem->AddMappingContext(UI, 0);
            }
        }

        // ★ 핵심: 입력 모드를 Game and UI로 바꾸고 마우스 커서 살리기
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(Widget->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(InputMode);
        SetShowMouseCursor(true);
    	Widget->SetUserFocus(this);   // 추가
    	UGameplayStatics::SetGamePaused(this, true);
    }
}

int32 AFebriculaPlayerController::GetWidgetCount()
{
	UE_LOG(LogTemp, Warning, TEXT("GetWidgetCount() %d"), OpenedWidgets.Num());
	return OpenedWidgets.Num();
}

bool AFebriculaPlayerController::RemoveWidget()
{
	if (OpenedWidgets.Num() > 0)
	{
		UUserWidget* LastWidget = OpenedWidgets.Pop();
		UE_LOG(LogTemp, Warning, TEXT("Removing Widget %s"), *LastWidget->GetName());
		UE_LOG(LogTemp, Warning, TEXT("RemoveWidget() %d"), OpenedWidgets.Num());
		GetWidgetCount();

		if (LastWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("지우기 전 IsInViewport: %s"), LastWidget->IsInViewport() ? TEXT("True") : TEXT("False"));

			LastWidget->SetVisibility(ESlateVisibility::Hidden);

			UE_LOG(LogTemp, Warning, TEXT("지운 후 IsInViewport: %s"), LastWidget->IsInViewport() ? TEXT("True") : TEXT("False"));
			UE_LOG(LogTemp, Warning, TEXT("지운 후 IsVisible: %s"), LastWidget->IsVisible() ? TEXT("True") : TEXT("False"));
		}
	}

    // 모든 위젯이 다 닫혀서 스택이 완전히 비워졌다면 게임플레이 상태로 복구
	if (OpenedWidgets.IsEmpty())
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
        	InputSubsystem->ClearAllMappings();
            // for (UInputMappingContext* Default : DefaultMappingContexts)
            // {
            //     InputSubsystem->AddMappingContext(Default, 0);
            // }
        	
        	if (UGameProgressSubsystem* Progress = GetWorld()->GetGameInstance()->GetSubsystem<UGameProgressSubsystem>())
        	{
        		InputSubsystem->AddMappingContext(Progress->GetInputMappingContext(), 0);
        	}

        	if (!ShouldUseTouchControls())
        	{
        		for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
        		{
        			InputSubsystem->AddMappingContext(CurrentContext, 0);
        		}
        	}
        	
        	UGameplayStatics::SetGamePaused(this, false);
        }

        // ★ 핵심: 입력 모드를 다시 Game Only로 복구하고 마우스 숨기기
        FInputModeGameOnly InputMode;
        SetInputMode(InputMode);
		SetShowMouseCursor(false);

    	if (AWorldSettings* WorldSettings = GetWorldSettings())
    	{
    		if (ALevelSetting* LevelSetting = Cast<ALevelSetting>(WorldSettings))
    		{
    			if (LevelSetting->bKeepCursor)
    			{
    				SetShowMouseCursor(true);
					if (IsValid(DefaultWidget))
					{
						FInputModeGameAndUI Mode;
						Mode.SetWidgetToFocus(DefaultWidget->TakeWidget());
						SetInputMode(Mode);
					}
    			}
    		}
    	}
		if (UDialogueSubsystem* Dialogue = GetWorld()->GetSubsystem<UDialogueSubsystem>())
		{
			if (Dialogue->IsDialogueActive())
			{
				if (Dialogue->GetPendingChoiceNum() > 0)
				{
					FInputModeGameAndUI Mode;
					if (ACharacter* Char = GetCharacter())
					{
						if (UInteractionComponent* InteractionComponent = Char->GetComponentByClass<UInteractionComponent>())
						{
							Mode.SetWidgetToFocus(InteractionComponent->GetHUDWidget()->TakeWidget());
						}
					}
					SetInputMode(Mode);
					SetShowMouseCursor(true);
					
					return true;
				}
				FInputModeGameOnly Mode;
				SetInputMode(Mode);
			}
			return true;
		}
    }
    return false;
}


void AFebriculaPlayerController::HandleRemove(const FInputActionValue& InputActionValue)
{
	UE_LOG(LogTemp, Warning, TEXT("HandleRemove() %d"), OpenedWidgets.Num());
	
	
	if (OpenedWidgets.Num() > 0)
	{
		RemoveWidget();
	}
	else
	{
		if (UGameProgressSubsystem* Progress = GetWorld()->GetGameInstance()->GetSubsystem<UGameProgressSubsystem>())
		{
			AddWidget(Progress->GetPauseWidget());
		}
	}
}

void AFebriculaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (IsLocalPlayerController() && ShouldUseTouchControls())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogFebricula, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
	
	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent))
	{
		Input->BindAction(UIAction, ETriggerEvent::Started, this, &AFebriculaPlayerController::HandleRemove);
	}
	
	if (UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (UGameProgressSubsystem* Progress = GetWorld()->GetGameInstance()->GetSubsystem<UGameProgressSubsystem>())
		{
			LocalPlayerSubsystem->RemoveMappingContext(Progress->GetInputMappingContext());
			LocalPlayerSubsystem->AddMappingContext(Progress->GetInputMappingContext(), 0);
		}
	}
	if (UDialogueSubsystem* Dialogue = GetWorld()->GetSubsystem<UDialogueSubsystem>())
	{
		Dialogue->ApplyPreDirection(this, EPreDirection::InitialFadeOut);
	}

}

void AFebriculaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

bool AFebriculaPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
