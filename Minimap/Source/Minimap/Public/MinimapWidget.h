// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "MinimapWidget.generated.h"

class AMinimapVolume;
class UTexture2D;

/**
 * 
 */
UCLASS()
class MINIMAP_API UMinimapWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	void UpdateTransform() const;

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	                          const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	                          const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	virtual uint32 PaintMinimap(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, uint32 LayerId) const;
	virtual uint32 PaintActors(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, uint32 LayerId) const;

	virtual bool GetPlayerView(FVector& Location, FRotator& Rotation) const;

	UFUNCTION(BlueprintCallable, Category = "MinimapWidget")
	void ChangeZoom(bool bIncreaseZoom);
	
protected:
	mutable FBox2D WidgetRect;
	mutable FTransform2d WorldToScreen;
	mutable FTransform2d ScreenToWorld;
	mutable FVector2D Trans;

	UPROPERTY()
	FSlateBrush WorldMiniMapBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap")
	float MinimapZoomLevel = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap")
	float ZoomLevelDifference = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap")
	float MinZoomLevel = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap")
	float MaxZoomLevel = 5000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap")
	TObjectPtr<AMinimapVolume> MinimapVolume;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap")
	TObjectPtr<UTexture2D> MinimapTexture;;
};
