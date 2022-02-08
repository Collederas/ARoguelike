#pragma once

#include "CoreMinimal.h"
#include "Roguelike.generated.h"

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
USTRUCT()
struct FGrid
{
	GENERATED_BODY()

	FGrid(): GridSizeX(1), GridSizeY(1) {}
	FGrid(int8 NumberOfRooms, FVector2D RoomDimensions)
	{
	    GridSizeX = NumberOfRooms * RoomDimensions.X;
	    GridSizeY = NumberOfRooms * RoomDimensions.Y;
	    
		Cells.Init(0, GridSizeX * GridSizeY);
	}
	TArray<FVector2D> RoomsCoordinates;

	FVector2D GetCentralRoomCoord() const
	{
		return FVector2D(FMath::Floor(GridSizeX/2), FMath::Floor(GridSizeY/2));
	}
	
	bool AddRoom(FVector2D RoomCoord)
	{
		if (IsRoom(RoomCoord))
			return false;
		
		int d = GridSizeX * RoomCoord.Y + RoomCoord.X;
		Cells[d] = 1;
		RoomsCoordinates.Add(RoomCoord);
		return true;
	}

	// Check value at given coord. Either a room (true) or not (false)
	bool IsRoom(const FVector2D Coord)
	{
	    int d = GridSizeX * Coord.Y + Coord.X;
	    if (Cells.Num() < d)
	        return false;
		return static_cast<bool>(Cells[d]);
	}
	
	// Populate Result with cardinal locations of adjacent rooms
	void GetAdjacentRoomCardinalPoints(TArray<CardinalPoint>& Result, const FVector2D Coord)
	{
		if (!IsRoom(Coord))
		{
			UE_LOG(LogTemp, Log, TEXT("No room at %s"), *Coord.ToString());
			return;
		}

		TMap<CardinalPoint, FVector2D> CardinalToCoordMap = {
			{North, FVector2D(Coord.X, Coord.Y + 1)},
			{South, FVector2D(Coord.X, Coord.Y - 1)},
			{East, FVector2D(Coord.X + 1, Coord.Y)},
			{West, FVector2D(Coord.X - 1, Coord.Y)}
		};

		for (auto CoordMap : CardinalToCoordMap)
		{
			if (IsRoom(CoordMap.Value))
			{
				Result.Add(CoordMap.Key);
			}
		}
	}

	void PrintDebug()
	{
		FString Output;
		for (int Idx = 0; Idx < Cells.Num(); Idx++)
		{
			if (Idx % GridSizeX == 0)
			{
				Output += FString("\n");
			}
			Output += FString::Printf(TEXT("%d, "), Cells[Idx]);
		}

		UE_LOG(LogTemp, Warning, TEXT("%s"), *Output);
	}
	
protected:
	// Using a bit of memory to make cardinal adjacency lookups a bit easier for me.
	// Cells is an array of all grid cells. Value is 0 if cell is empty, 1 if it
	// contains a room.
	TArray<int8> Cells;

private:
	int8 GridSizeX;
	int8 GridSizeY;
};