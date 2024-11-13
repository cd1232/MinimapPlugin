// Craig Duthie 2024


#include "MinimapCapture.h"

#include "AssetCompilingManager.h"
#include "IAssetTools.h"
#include "Components/SceneCaptureComponent2D.h"
#include "AssetRegistry/AssetRegistryModule.h"

#include "MinimapVolume.h"
#include "SourceControlHelpers.h"
#include "UObject/SavePackage.h"
#include "WorldPartition/WorldPartitionMiniMapHelper.h"


// Sets default values
AMinimapCapture::AMinimapCapture()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComponent"));
	CaptureComponent->SetupAttachment(GetRootComponent());
}

void AMinimapCapture::CaptureMinimap()
{
	if (!MinimapVolume)
	{
		UE_LOG(LogTemp, Error, TEXT("CaptureMinimap: No minimap volume set"));
		return;
	}
		
	SetActorLocation(MinimapVolume->GetBounds().Origin);

	FBox WorldBounds = MinimapVolume->GetBounds().GetBox();

	int MinimapImageSizeX = WorldBounds.GetSize().X / WorldUnitsPerPixel;
	int MinimapImageSizeY = WorldBounds.GetSize().Y / WorldUnitsPerPixel;
	MinimapImageSizeX = FMath::Min(MinimapImageSizeX, UTexture::GetMaximumDimensionOfNonVT());
	MinimapImageSizeY = FMath::Min(MinimapImageSizeY, UTexture::GetMaximumDimensionOfNonVT());
	
	int OutWorldUnitsPerPixel = FMath::CeilToInt(FMath::Max(WorldBounds.GetSize().X / MinimapImageSizeX, WorldBounds.GetSize().Y / MinimapImageSizeY));
	MinimapImageSizeX = WorldBounds.GetSize().X / OutWorldUnitsPerPixel;
	MinimapImageSizeY = WorldBounds.GetSize().Y / OutWorldUnitsPerPixel;

	// Compute world to minimap transform	
	FMatrix WorldToMinimap = FReversedZOrthoMatrix(WorldBounds.Min.X, WorldBounds.Max.X,
												   WorldBounds.Min.Y, WorldBounds.Max.Y, 1.0f, 0.0f);

	FVector3d Translation(WorldBounds.Max.X / WorldBounds.GetSize().X, WorldBounds.Max.Y / WorldBounds.GetSize().Y, 0);
	FVector3d Scaling(MinimapImageSizeX, MinimapImageSizeY, 1);

	WorldToMinimap *= FTranslationMatrix(Translation);
	WorldToMinimap *= FScaleMatrix(Scaling);	

	FString AssetName = FString("T_");
	AssetName.Append(GetWorld()->GetMapName());
	AssetName.Append(TEXT("_Minimap"));
	FString PackageName = *MinimapPackagePath.Append(TEXT("/")).Append(AssetName);
	
	UPackage* Package = CreatePackage(*PackageName);
	
	FName TextureName = MakeUniqueObjectName(Package, UTexture2D::StaticClass(), FName(*AssetName));
	UTexture2D* MiniMapTexture = NewObject<UTexture2D>(Package, UTexture2D::StaticClass(), *AssetName, RF_Public | RF_Standalone);
	MiniMapTexture->PreEditChange(nullptr);
	MiniMapTexture->Source.Init(MinimapImageSizeX, MinimapImageSizeY, 1, 1, TSF_BGRA8);
	MiniMapTexture->MipGenSettings = TMGS_SimpleAverage;
	
	FAssetRegistryModule::AssetCreated(MiniMapTexture);
	Package->MarkPackageDirty();

	
	FWorldPartitionMiniMapHelper::CaptureBoundsMiniMapToTexture(GetWorld(), Package, MinimapImageSizeX, MinimapImageSizeY,
		MiniMapTexture, AssetName, MinimapVolume->GetBounds().GetBox(), SCS_BaseColor, 5);

	// Post
	FAssetCompilingManager::Get().FinishAllCompilation();
	
	MiniMapTexture->PowerOfTwoMode = ETexturePowerOfTwoSetting::PadToPowerOfTwo;
	MiniMapTexture->AdjustMinAlpha = 1.f;
	MiniMapTexture->LODGroup = TEXTUREGROUP_UI;
	MiniMapTexture->VirtualTextureStreaming = false;
	MiniMapTexture->PostEditChange();

	FAssetCompilingManager::Get().FinishAllCompilation();

	FString PackageFileName = SourceControlHelpers::PackageFilename(Package);
	Package->FullyLoad();

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Standalone;
	if (!UPackage::SavePackage(Package, nullptr, *PackageFileName, SaveArgs))
	{
		//UE_LOG(LogTemp, Error, TEXT("Error saving package %s."), *WorldMiniMapExternalPackage->GetName());
	}
}

