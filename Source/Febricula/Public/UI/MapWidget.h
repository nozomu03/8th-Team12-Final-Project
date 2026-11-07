#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Cache/DisplayPoint.h"
#include "MapWidget.generated.h"

class UMapButton;
class UButton;
class UCanvasPanelSlot;
class UImage;
class UCanvasPanel;
class UOverlay;
class UMaterialInterface;

UCLASS()
class FEBRICULA_API UMapWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(
		const FGeometry& MyGeometry,
		float InDeltaTime
	) override;

	void RefreshDisplayPoints();
	void OnMapButtonClicked(FVector Location);
	void CreatePoiButtons();
	void UpdatePoiPositions();
	void RemoveUnusedPoiButtons();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> Overlay_MapViewport;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> Canvas_Map;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Map;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Player;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> Canvas_POI;

	UPROPERTY()
	TMap<FName, TObjectPtr<UMapButton>> POIButtons;
	
	FVector2D CurrentMapPosition = FVector2D::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TObjectPtr<UMaterialInterface> MapMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	FVector2D MapWorldMin = FVector2D(-1740.025f, -10301.714f);

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	FVector2D MapWorldMax = FVector2D(12490.497f, 3928.809f);

	// 테스트가 끝나면 false로 변경
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	bool bMapTestMode = false;
	
	UPROPERTY(VisibleAnywhere, Category = "Map")
	TArray<FDisplayPoint> DisplayPoints;

private:
	UPROPERTY()
	TObjectPtr<UCanvasPanelSlot> MapImageSlot;

	FVector2D MapPixelSize = FVector2D::ZeroVector;

	bool bPendingMapRefresh = false;

	void UpdateMapPosition();

	void UpdateMapPosition(const FVector& WorldLocation);

public:
	UMapWidget(const FObjectInitializer& OI);
	// 공용 UI 시스템에서 맵을 다시 표시할 때 호출
	void RequestMapRefresh();
};