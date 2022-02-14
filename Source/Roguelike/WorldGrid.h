#pragma once

#include "CoreMinimal.h"
#include "WorldGrid.generated.h"

/**
 * Useful for room to room spatial relation
 */
UENUM()
enum CardinalPoint
{
	North,
	South,
	East,
	West
};

/**
 * Represents a Grid as a single dimensional array. Indices start at 0 for both
 * X and Y.
 */
UCLASS()
class AGrid: public AActor
{
	GENERATED_BODY()

public:
	AGrid(): RoomDimensions(16), GridSize(FIntPoint(80)) {}
	virtual void Init(FVector2D InRoomDimensions)
	{
		Empty();
		RoomDimensions = InRoomDimensions;		
	    GridSize.X = NumberOfRooms.X * RoomDimensions.X;
	    GridSize.Y = NumberOfRooms.Y * RoomDimensions.Y;
		
		RoomTiles.Init(0, NumberOfRooms.X * NumberOfRooms.Y);
	}

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int GridTileSize = 200;
	
	TArray<FVector2D> RoomsCoordinates;

	FVector GetWorldLocationForGridCell(const FVector2D& Pos) const;
	bool GetGridCellForWorldLocation(const FVector& WorldPos, FVector2D& GridPos) const;
	FVector GetWorldLocationForGridCellCentre(const FIntPoint& pos) const;
	virtual FVector2D GetRoomCoord(FVector WorldCoord); 

	FVector2D GetCentralRoomCoord() const;
	virtual FVector2D GetRandomPointInRoom(FVector2D RoomCoord); 
	
	
	bool AddRoom(FVector2D RoomCoord);

	// Check value at given coord. Either a room (true) or not (false)
	bool IsRoom(const FVector2D Coord);
	
	// Populate Result with cardinal locations of adjacent rooms
	void GetAdjacentRoomCardinalPoints(TArray<CardinalPoint>& Result, const FVector2D Coord);

	void PrintRoomsLayoutDebug();

	bool IsGridCellWalkable(const FIntPoint& Location) const;

	void AddBlockedTile(FIntPoint Location);
	
	bool IsValidGridCell(const FIntPoint& Location) const;
	FVector2D GetRoomOrigin(FVector2D RoomGridCoord);

	// Clears all instance data
	virtual void Empty();

	UPROPERTY(EditDefaultsOnly)
	TArray<FIntPoint> BlockedTiles;

protected:
	UPROPERTY(BlueprintReadOnly)
	FVector2D NumberOfRooms = FVector2D(10);
	
	UPROPERTY(BlueprintReadOnly)
	FVector2D RoomDimensions;
	// It is an array of macro-tiles (size: (RoomDimensions.X, RoomDimensions.Y)),
	// 1 for each potential room position. Value is 0 if tile is empty, 1 if it contains a room.
	TArray<int8> RoomTiles;
private:
	FIntPoint GridSize;
};