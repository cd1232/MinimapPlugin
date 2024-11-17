// Craig Duthie 2024


#include "MinimapWidget.h"

#include "EngineUtils.h"

#include "MinimapVolume.h"


void UMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!MinimapVolume)
	{
		for (TActorIterator<AActor> It(GetWorld(), AVolume::StaticClass()); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor->Tags.Contains("MinimapVolume"))
			{
				MinimapVolume = Cast<AMinimapVolume>(Actor);
			}
		}
	}

	if (!MinimapVolume)
	{
		UE_LOG(LogTemp, Error, TEXT("MinimapWidget Cannot find MinimapVolume. Please make sure to tag a volume with MinimapVolume"));
		return;
	}

	if (MinimapTexture)
	{
		int32 MinimapImageSizeX = MinimapTexture->GetImportedSize().X;
		int32 MinimapImageSizeY = MinimapTexture->GetImportedSize().Y;
		
		// Compute relevant UV space for the minimap		
		FVector2D TexturePOW2ScaleFactor = FVector2D((float)MinimapImageSizeX / FMath::RoundUpToPowerOfTwo(MinimapImageSizeX),
			(float)MinimapImageSizeY / FMath::RoundUpToPowerOfTwo(MinimapImageSizeY));
		
		FBox2D UVOffset(FVector2d(0,0), TexturePOW2ScaleFactor);
		
	    WorldMiniMapBrush.SetUVRegion(UVOffset);
		WorldMiniMapBrush.SetImageSize(MinimapTexture->GetImportedSize());
		WorldMiniMapBrush.SetResourceObject(MinimapTexture);
	}	
}

void UMinimapWidget::UpdateTransform() const
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		FVector ViewPosition;
		FRotator ViewRotation;				
		GetPlayerView(ViewPosition, ViewRotation);

		FBox Box = MinimapVolume->GetBounds().GetBox();

		const FVector2D PlayerExtent = FVector2D(MinimapZoomLevel);
		FTransform2d T = FTransform2d(1.0f, FVector2D(-ViewPosition.X, -ViewPosition.Y));
		
		const FTransform2d V = FTransform2d((WidgetRect.GetExtent() / PlayerExtent).GetMin(), FVector2D(WidgetRect.GetSize().X * 0.5f, WidgetRect.GetSize().Y * 0.5f));
		
		WorldToScreen = T.Concatenate(V);
		ScreenToWorld = WorldToScreen.Inverse();		
	}
}

int32 UMinimapWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
                                  const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
                                  const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	WidgetRect = FBox2D(FVector2D(0, 0), AllottedGeometry.GetLocalSize());

	UpdateTransform();

	LayerId = PaintMinimap(AllottedGeometry, MyCullingRect, OutDrawElements, ++LayerId);
	LayerId = PaintActors(AllottedGeometry, MyCullingRect, OutDrawElements, ++LayerId);
	
	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
	                          bParentEnabled);
}

uint32 UMinimapWidget::PaintMinimap(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements, uint32 LayerId) const
{
	if (MinimapTexture)
	{
		if (MinimapVolume)
		{
			FBox WorldMinimapBounds = MinimapVolume->GetBounds().GetBox();
		
			const FBox2D MinimapBounds(
						WorldToScreen.TransformPoint(FVector2D(WorldMinimapBounds.Min)),
						WorldToScreen.TransformPoint(FVector2D(WorldMinimapBounds.Max))
					);

			const FPaintGeometry WorldImageGeometry = AllottedGeometry.ToPaintGeometry(
				MinimapBounds.GetSize(),
			FSlateLayoutTransform(MinimapBounds.Min)
			);

			FSlateDrawElement::MakeBox(
				OutDrawElements,
				++LayerId,
				WorldImageGeometry,
				&WorldMiniMapBrush,
				ESlateDrawEffect::None,
				USlateThemeManager::Get().GetColor(EStyleColor::White)
			);
		}
	}
	
	return ++LayerId;
}

uint32 UMinimapWidget::PaintActors(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
                                   FSlateWindowElementList& OutDrawElements, uint32 LayerId) const
{
	if (!MinimapVolume)
	{
		return LayerId;
	}
	
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		const FSlateBrush* CameraImage = FAppStyle::GetBrush(TEXT("Sequencer.KeyCircle"));

		FVector ViewPosition;
		FRotator ViewRotation;				
		GetPlayerView(ViewPosition, ViewRotation);
		
		const FVector2D LocalLocation = WorldToScreen.TransformPoint(FVector2D(ViewPosition));
		const FPaintGeometry PaintGeometryShadow = AllottedGeometry.ToPaintGeometry(
			CameraImage->ImageSize,
			FSlateLayoutTransform(WidgetRect.GetSize() * 0.5f - (CameraImage->ImageSize) * 0.5f)
		);

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			++LayerId,
			PaintGeometryShadow,
			CameraImage,
			ESlateDrawEffect::None,
			USlateThemeManager::Get().GetColor(EStyleColor::AccentYellow)
		);
	}

	return ++LayerId;
}

bool UMinimapWidget::GetPlayerView(FVector& Location, FRotator& Rotation) const
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		PlayerController->GetPlayerViewPoint(Location, Rotation);
	}

	return false;
}

void UMinimapWidget::ChangeZoom(bool bIncreaseZoom)
{
	if (bIncreaseZoom)
	{
		MinimapZoomLevel = FMath::Clamp(MinimapZoomLevel - ZoomLevelDifference, MinZoomLevel, MaxZoomLevel);
	}
	else
	{
		MinimapZoomLevel = FMath::Clamp(MinimapZoomLevel + ZoomLevelDifference, MinZoomLevel, MaxZoomLevel);
	}
}
