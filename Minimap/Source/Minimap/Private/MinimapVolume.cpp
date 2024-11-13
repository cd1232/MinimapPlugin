// Craig Duthie 2024


#include "MinimapVolume.h"

AMinimapVolume::AMinimapVolume()
{
	Tags.Add("MinimapVolume");
}

FVector2D AMinimapVolume::GetCenter() const
{
	FBox Box(ForceInit);
	Box = GetBounds().GetBox();

	const FBox2D Box2D(FVector2D(Box.Min), FVector2D(Box.Max));
	return Box2D.GetCenter();
}
