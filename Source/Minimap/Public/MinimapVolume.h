// Craig Duthie 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "MinimapVolume.generated.h"

/**
 * 
 */
UCLASS()
class MINIMAP_API AMinimapVolume : public AVolume
{
	GENERATED_BODY()

public:
	AMinimapVolume();
	
	UFUNCTION(BlueprintPure, Category = "MinimapVolume")
	FVector2D GetCenter() const;
};
