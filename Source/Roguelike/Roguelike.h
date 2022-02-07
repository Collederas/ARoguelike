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
	FGrid(int8 InGridSizeX, int8 InGridSizeY): GridSizeX(InGridSizeX), GridSizeY(InGridSizeY)
	{
		Cells.Init(0, InGridSizeX * InGridSizeY);
	}

	// Using a bit of memory to make cardinal adjacency lookups a bit easier for me.
	// Cells is an array of all grid cells with 0 if they are empty and 1 if they contain
	// a room.
	TArray<int8> Cells;
	TArray<FVector2D> RoomsCoordinates;

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
		return static_cast<bool>(Cells[GridSizeX * Coord.Y + Coord.X]);
	}
	
	// Populate Result with array of cardinal locations of adjacent rooms
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
		for (int idx = 0; idx < Cells.Num(); idx++)
		{
			if (idx % GridSizeX == 0)
			{
				Output += FString("\n");
			}
			Output += FString::Printf(TEXT("%d, "), Cells[idx]);
		}

		UE_LOG(LogTemp, Warning, TEXT("%s"), *Output);
	}
	
private:
	int8 GridSizeX;
	int8 GridSizeY;
};