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
	RoomsCoord.Add(CurrentRoomCoord);

	// Now add the remaining rooms
	for (uint8 Idx = 0; Idx < RoomNr - 1; Idx++)
	{
		CurrentRoomCoord = SelectAdjacentRoomCoord(CurrentRoomCoord);
		RoomsCoord.Add(CurrentRoomCoord);
	}

	for (auto RoomCoord : RoomsCoord)
	{
		FString SourceImagePath = FString("C:/Users/Marco/Documents/Unreal Projects/Roguelike/Content/ImgTest.png");
		UImageProcessor* ImageProcessor = NewObject<UImageProcessor>();

		TArray<FVector> Image;
		int Width, Height;
		
		ImageProcessor->LoadImage(Image, Width, Height, SourceImagePath);

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



