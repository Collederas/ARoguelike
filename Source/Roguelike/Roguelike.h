#pragma once

#include "CoreMinimal.h"
#include "Roguelike.generated.h"


USTRUCT()
struct FRoom
{
	GENERATED_BODY()

	UPROPERTY()
	FVector2D Coord;

public:
	bool operator==(const FRoom& Other) const;

	// virtual FVector2D GetOriginWorldSpace(FVector2D RoomCoords)
	// {
	// 	return FVector2D(RoomWidthUnits * RoomCoords.X, RoomHeightUnits * RoomCoords.Y) * UnitScale;
	// }
};

UCLASS()
class UGrid : public UObject
{
	GENERATED_BODY()
	
	TArray<FRoom> Rooms;
};