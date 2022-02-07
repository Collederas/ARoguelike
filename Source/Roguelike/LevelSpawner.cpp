// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSpawner.h"

#include "ImageProcessor.h"

// Sets default values
ALevelSpawner::ALevelSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ALevelSpawner::BeginPlay()
{
	Super::BeginPlay();

	// First room is always going to be in the center of the grid
	const float GridMidPoint = FMath::Floor(GridSizeUnits/2);
	FVector2D CurrentRoomCoord = FVector2D(GridMidPoint, GridMidPoint);
	Rooms.Add(CurrentRoomCoord);

	// Now add the remaining rooms
	for (uint8 Idx = 0; Idx < RoomNr - 1; Idx++)
	{
		CurrentRoomCoord = SelectAdjacentRoomCoord(CurrentRoomCoord);
		Rooms.Add(CurrentRoomCoord);
	}

	for (int RoomIndex = 0; Rooms.Num() -1; RoomIndex++)
	{
		if (RoomIndex == 1)
		{
			// First room should be empty?		
		}

		if (RoomIndex == Rooms.Num() - 1)
		{
			// Last room should contain the exit		
		}

		FVector2D RoomOriginWS = GetRoomOriginWorldSpace(Rooms[RoomIndex]);
		
		const FString SourceImagePath = GetRandomSourceImage();
		UImageProcessor* ImageProcessor = NewObject<UImageProcessor>();
		TArray<FVector> Image;
		int Width, Height;
		
		ImageProcessor->LoadImage(Image, Width, Height, SourceImagePath);

		FVector2D PixelCoord = FVector2D::ZeroVector;
		for (int PixelIndex = 0; PixelIndex < Image.Num() - 1; PixelIndex++)
		{
			PixelCoord.X = PixelIndex % RoomWidthUnits;

			// Every time we wrap around, increase Pixel Y coord
			if (PixelCoord.X == 0 && PixelIndex > 0)
				PixelCoord.Y ++;  // This means that this coordinate starts at 0 (eg.: on a 16x16px image, the max Y is 15)
			
			// Outer walls detection for room connection/sealing logic
			if(PixelCoord.X == 0)
			{
				// West wall
			}

			if (PixelCoord.X == RoomWidthUnits - 1)
			{
				// East wall
			}

			if (PixelCoord.Y == 0)
			{
				// South wall
			}

			if (PixelCoord.Y == RoomHeightUnits - 1)
			{
				// North wall
			}
			
			
			
		}
		// -- WIP Test Image data --
		int x = 2;
		int y = 0;
		size_t index = y * Width + x;
		UE_LOG(LogTemp, Warning, TEXT("RGB pixel @ (x=%d, y=%d): %s"), x, y, *Image[index].ToString());
		// -------------------------
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

FVector2D ALevelSpawner::GetRoomOriginWorldSpace(FVector2D RoomCoords)
{
	return FVector2D(RoomWidthUnits * RoomCoords.X, RoomHeightUnits * RoomCoords.Y + RoomHeightUnits) * UnitScale;
}


