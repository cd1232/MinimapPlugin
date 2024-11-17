// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinimapCapture.generated.h"

class AMinimapVolume;

UCLASS(config = Game)
class MINIMAP_API AMinimapCapture : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMinimapCapture();

	UFUNCTION(CallInEditor, BlueprintCallable)
	void CaptureMinimap();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap")
	TObjectPtr<USceneCaptureComponent2D> CaptureComponent;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<AMinimapVolume> MinimapVolume;

	UPROPERTY(EditAnywhere)
	int32 WorldUnitsPerPixel = 50;

	UPROPERTY(EditAnywhere, config, Category = Minimap)
	FString MinimapPackagePath;
};
