// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSpawner.h"

#include "ImageProcessor.h"
#include "Roguelike.h"

// Sets default values
ALevelSpawner::ALevelSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ALevelSpawner::BeginPlay()
{
	Super::BeginPlay();

	FGrid WorldGrid(GridSizeUnits, GridSizeUnits);
	UImageProcessor* ImageProcessor = NewObject<UImageProcessor>();
	
	// First room is always going to be in the center of the grid
	const int GridMidPoint = FMath::Floor(GridSizeUnits/2);
	FVector2D CurrentRoomCoord = FVector2D(GridMidPoint, GridMidPoint);
	WorldGrid.AddRoom(CurrentRoomCoord);
	
	// Now add the remaining rooms
	for (uint8 Idx = 1; Idx < RoomNr; Idx++)
	{
		bool ValidRoomFound = false;
		FVector2D ChosenRoomCoord;
		
		while (!ValidRoomFound)
		{
			ChosenRoomCoord = SelectAdjacentRoomCoord(CurrentRoomCoord);
			ValidRoomFound = !WorldGrid.IsRoom(ChosenRoomCoord);
		}
			CurrentRoomCoord = ChosenRoomCoord;
			WorldGrid.AddRoom(ChosenRoomCoord);
	}

	for (int RoomIndex = 0; RoomIndex < WorldGrid.RoomsCoordinates.Num(); RoomIndex++)
	{
		if (RoomIndex == 1)
		{
			// First room should be empty?	
		}
	
		if (RoomIndex == Rooms.Num() - 1)
		{
			// Last room should contain the exit		
		}
		
		const FVector2D RoomCoord = WorldGrid.RoomsCoordinates[RoomIndex];
		TArray<CardinalPoint> AdjacentRoomsCardinalPoints;
		WorldGrid.GetAdjacentRoomCardinalPoints(AdjacentRoomsCardinalPoints, RoomCoord);
	 	
		const FString SourceImagePath = GetRandomSourceImage();
		TArray<FVector> Image;
		int Width, Height;
		ImageProcessor->LoadImage(Image, Width, Height, SourceImagePath);
	
		FVector2D PixelCoord = FVector2D::ZeroVector;
		for (int PixelIndex = 0; PixelIndex < Image.Num(); PixelIndex++)
		{
			PixelCoord.X = PixelIndex % RoomWidthUnits;
	
			// Every time we wrap around, increase Pixel Y coord
			if (PixelCoord.X == 0 && PixelIndex > 0)
				PixelCoord.Y ++;  // This means that this coordinate starts at 0 (eg.: on a 16x16px image, the max Y is 15)

			FVector SpawnLocation = FVector(GetRoomOrigin(RoomCoord) + PixelCoord, 0) * UnitScale;
			
			// Outer walls detection for room connection/sealing logic
			if (PixelCoord.X == 0 && !AdjacentRoomsCardinalPoints.Contains(West))
			{
				// West wall
				GetWorld()->SpawnActor(WallActor, &SpawnLocation, NULL, FActorSpawnParameters());
			}
	
			if (PixelCoord.X == RoomWidthUnits - 1 && !AdjacentRoomsCardinalPoints.Contains(East))
			{
				// East wall
				GetWorld()->SpawnActor(WallActor, &SpawnLocation, NULL, FActorSpawnParameters());
			}
	
			if (PixelCoord.Y == 0 && !AdjacentRoomsCardinalPoints.Contains(South))
			{
				// South wall
				GetWorld()->SpawnActor(WallActor, &SpawnLocation, NULL, FActorSpawnParameters());
			}
	
			if (PixelCoord.Y == RoomHeightUnits - 1 && !AdjacentRoomsCardinalPoints.Contains(North))
			{
				// North wall
				GetWorld()->SpawnActor(WallActor, &SpawnLocation, NULL, FActorSpawnParameters());
			}
			
			
			
		}
		// 	// -- WIP Test Image data --
		// 	int x = 2;
		// 	int y = 0;
		// 	size_t index = y * Width + x;
		// 	// UE_LOG(LogTemp, Warning, TEXT("RGB pixel @ (x=%d, y=%d): %s"), x, y, *Image[index].ToString());
		// 	// -------------------------
		// }
	}
}

FVector2D ALevelSpawner::SelectAdjacentRoomCoord(const FVector2D RoomCoord)
{
	TArray<FVector2D> AvailableChoices;
	AvailableChoices.Add(FVector2D(RoomCoord.X + 1, RoomCoord.Y));
	AvailableChoices.Add(FVector2D(RoomCoord.X, RoomCoord.Y + 1));
	AvailableChoices.Add(FVector2D(RoomCoord.X - 1, RoomCoord.Y));
	AvailableChoices.Add(FVector2D(RoomCoord.X, RoomCoord.Y - 1));

	const int RandomIndex = FMath::FRandRange(0, 3);
	
	return AvailableChoices[RandomIndex];
}

FString ALevelSpawner::GetRandomSourceImage()
{
	const int RandomIndex = FMath::FRandRange(0, RoomSourceImages.Num() - 1);
	return RoomSourceImages[RandomIndex].FilePath;
}

FVector2D ALevelSpawner::GetRoomOrigin(FVector2D RoomGridCoord)
{
	return FVector2D(RoomWidthUnits * RoomGridCoord.X, RoomHeightUnits * RoomGridCoord.Y);
}



