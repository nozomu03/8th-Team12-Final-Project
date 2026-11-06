#include "UI/MapWidget.h"

#include "Cache/ActorCacheSubsystem.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"

#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

// 실제 프로젝트의 ActorCacheSubsystem 헤더 경로에 맞춰 수정


void UMapWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!Overlay_MapViewport ||
        !Canvas_Map ||
        !Canvas_POI ||
        !Image_Map ||
        !Image_Player)
    {
        return;
    }

    // 지도 이미지 영역 밖 클리핑
    Overlay_MapViewport->SetClipping(
        EWidgetClipping::ClipToBounds
    );

    // 지도 머티리얼 적용
    if (MapMaterial)
    {
        Image_Map->SetBrushFromMaterial(MapMaterial);
    }

    // 지도 이미지 슬롯 확보
    MapImageSlot = Cast<UCanvasPanelSlot>(
        Image_Map->Slot
    );

    if (MapImageSlot)
    {
        MapImageSlot->SetAnchors(
            FAnchors(0.f, 0.f)
        );

        MapImageSlot->SetAlignment(
            FVector2D(0.f, 0.f)
        );
    }
}


void UMapWidget::NativeTick(
    const FGeometry& MyGeometry,
    float InDeltaTime
)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!bPendingMapRefresh)
    {
        return;
    }

    const FVector2D ViewportSize =
        Overlay_MapViewport->GetCachedGeometry().GetLocalSize();

    const FVector2D MapSize =
        MapImageSlot->GetSize();

    // 레이아웃이 아직 준비되지 않았다면 다음 Tick에 재시도
    if (ViewportSize.IsNearlyZero() ||
        MapSize.IsNearlyZero())
    {
        return;
    }

    // 여기까지 왔으면 갱신 가능
    bPendingMapRefresh = false;

    RefreshDisplayPoints();
    UpdateMapPosition();
}

void UMapWidget::RequestMapRefresh()
{
    bPendingMapRefresh = true;
}


// --------------------------------------------------
// POI 데이터 및 버튼 관리
// --------------------------------------------------

void UMapWidget::RefreshDisplayPoints()
{
    UActorCacheSubsystem* Cache =
        GetWorld()->GetSubsystem<UActorCacheSubsystem>();

    if (!Cache)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[Map] ActorCacheSubsystem is NULL"));

        return;
    }

    // MapDisplay 역할을 가진 TargetPoint 목록 수집
    DisplayPoints = Cache->GetTargetLocationByRole(
        ETargetTypeRole::MapDisplay
    );

    RemoveUnusedPoiButtons();
    CreatePoiButtons();
}


void UMapWidget::RemoveUnusedPoiButtons()
{
    for (auto It = POIButtons.CreateIterator(); It; ++It)
    {
        const FName CurrentPointName = It.Key();

        const bool bExists = DisplayPoints.ContainsByPredicate(
            [CurrentPointName](const FDisplayPoint& Point)
            {
                return Point.PointName == CurrentPointName;
            }
        );

        if (!bExists)
        {
            if (IsValid(It.Value()))
            {
                It.Value()->RemoveFromParent();
            }

            It.RemoveCurrent();
        }
    }
}


void UMapWidget::CreatePoiButtons()
{
    if (!Canvas_POI)
    {
        return;
    }

    for (const FDisplayPoint& Point : DisplayPoints)
    {
        // 이미 생성된 버튼이면 건너뜀
        if (POIButtons.Contains(Point.PointName))
        {
            continue;
        }

        UButton* NewButton = NewObject<UButton>(this);
        UTextBlock* ButtonText = NewObject<UTextBlock>(this);
        ButtonText->SetText(FText::FromName(Point.PointName));
        
        NewButton->AddChild(ButtonText);
        
        if (!NewButton)
        {
            continue;
        }

        UCanvasPanelSlot* ButtonSlot =
            Canvas_POI->AddChildToCanvas(NewButton);

        if (!ButtonSlot)
        {
            NewButton->RemoveFromParent();
            continue;
        }

        // 버튼 중심을 기준으로 위치 지정
        ButtonSlot->SetAlignment(
            FVector2D(0.5f, 0.5f)
        );

        // 버튼의 원하는 크기에 맞춰 자동 크기 사용
        ButtonSlot->SetAutoSize(true);

        POIButtons.Add(
            Point.PointName,
            NewButton
        );
    }
}


// --------------------------------------------------
// 지도 위치 계산
// --------------------------------------------------

void UMapWidget::TeleportPlayer(FVector Location)
{
    if (ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0))
    {
        
    }
}

void UMapWidget::UpdateMapPosition()
{
    APlayerController* PC = GetOwningPlayer();

    if (!PC)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[Map] OwningPlayer is NULL"));

        return;
    }

    APawn* PlayerPawn = PC->GetPawn();

    if (!PlayerPawn)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[Map] Pawn is NULL"));

        return;
    }

    if (!MapImageSlot)
    {
        return;
    }

    // 지도 이미지 슬롯 크기
    MapPixelSize = MapImageSlot->GetSize();

    if (MapPixelSize.IsNearlyZero())
    {
        return;
    }

    // 화면 표시 영역 크기
    const FVector2D ViewportSize =
        Overlay_MapViewport->GetCachedGeometry().GetLocalSize();

    if (ViewportSize.IsNearlyZero())
    {
        return;
    }

    // 월드 좌표 범위
    const float RangeX =
        MapWorldMax.X - MapWorldMin.X;

    const float RangeY =
        MapWorldMax.Y - MapWorldMin.Y;

    if (RangeX <= 0.f || RangeY <= 0.f)
    {
        return;
    }

    const FVector PlayerLocation =
        PlayerPawn->GetActorLocation();

    
    UE_LOG(LogTemp, Warning,
    TEXT("[Map] PlayerLocation: %s"),
    *PlayerLocation.ToString());
    
    // 월드 좌표를 지도 좌표로 변환
    const float U = FMath::Clamp(
        (PlayerLocation.Y - MapWorldMin.Y) / RangeY,
        0.f,
        1.f
    );

    const float V = 1.f - FMath::Clamp(
        (PlayerLocation.X - MapWorldMin.X) / RangeX,
        0.f,
        1.f
    );

    // 지도 이미지 내부의 플레이어 좌표
    const FVector2D PlayerMapPosition(
        U * MapPixelSize.X,
        V * MapPixelSize.Y
    );

    // 플레이어를 화면 중앙에 배치
    const FVector2D ViewportCenter =
        ViewportSize * 0.5f;

    CurrentMapPosition =
        ViewportCenter - PlayerMapPosition;

    UE_LOG(LogTemp, Warning,
    TEXT("[Map] MapPixelSize: %s | ViewportSize: %s | CurrentMapPosition: %s"),
    *MapPixelSize.ToString(),
    *ViewportSize.ToString(),
    *CurrentMapPosition.ToString());
    
    // 지도 이미지 이동
    MapImageSlot->SetPosition(CurrentMapPosition);
    
    UE_LOG(LogTemp, Warning,
        TEXT("[Map] AppliedPosition: %s"),
        *MapImageSlot->GetPosition().ToString());
    // POI 버튼 위치 갱신
    UpdatePoiPositions();
}


// --------------------------------------------------
// POI 버튼 위치 계산
// --------------------------------------------------

void UMapWidget::UpdatePoiPositions()
{
    if (!Canvas_POI)
    {
        return;
    }

    const FVector2D ViewportSize =
        Overlay_MapViewport->GetCachedGeometry().GetLocalSize();

    const float RangeX =
        MapWorldMax.X - MapWorldMin.X;

    const float RangeY =
        MapWorldMax.Y - MapWorldMin.Y;

    if (RangeX <= 0.f || RangeY <= 0.f)
    {
        return;
    }

    for (const FDisplayPoint& Point : DisplayPoints)
    {
        TObjectPtr<UButton>* FoundButton =
            POIButtons.Find(Point.PointName);

        if (!FoundButton || !IsValid(FoundButton->Get()))
        {
            continue;
        }

        // POI 월드 좌표를 지도 좌표로 변환
        const float U = FMath::Clamp(
            (Point.Location.Y - MapWorldMin.Y) / RangeY,
            0.f,
            1.f
        );

        const float V = 1.f - FMath::Clamp(
            (Point.Location.X - MapWorldMin.X) / RangeX,
            0.f,
            1.f
        );

        const FVector2D POIMapPosition(
            U * MapPixelSize.X,
            V * MapPixelSize.Y
        );

        // 지도 이미지와 동일한 이동량 적용
        const FVector2D ScreenPosition =
            POIMapPosition + CurrentMapPosition;

        const bool bInsideViewport =
            ScreenPosition.X >= 0.f &&
            ScreenPosition.X <= ViewportSize.X &&
            ScreenPosition.Y >= 0.f &&
            ScreenPosition.Y <= ViewportSize.Y;

        UButton* Button = FoundButton->Get();

        Button->SetVisibility(
            bInsideViewport
                ? ESlateVisibility::Visible
                : ESlateVisibility::Collapsed
        );

        if (!bInsideViewport)
        {
            continue;
        }

        UCanvasPanelSlot* ButtonSlot =
            Cast<UCanvasPanelSlot>(Button->Slot);

        if (ButtonSlot)
        {
            ButtonSlot->SetPosition(ScreenPosition);
        }
    }
}