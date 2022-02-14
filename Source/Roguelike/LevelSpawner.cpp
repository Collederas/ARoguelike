#include "LevelSpawner.h"
#include "ImageProcessor.h"
#include "WorldGrid.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/GridNavigationData.h"

ALevelSpawner::ALevelSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALevelSpawner::SpawnNewLevel()
{
	constexpr int MaxRoomNr = 20;
	
	if (RoomNr > MaxRoomNr)
	{
		// If linking room number to progression, maybe this could throw event for beating the game.
		UE_LOG(LogTemp, Warning, TEXT("Cannot spawn more than %d rooms. Aborting"), MaxRoomNr);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Spawning New Level with %d rooms"), RoomNr);

	for (auto SpawnedWall : SpawnedWalls)
	{
		SpawnedWall->Destroy();
	}

	SpawnedWalls.Empty();
	
	if (!Grid)
		Grid = (AGrid*)GetWorld()->SpawnActor(AGrid::StaticClass(), &FTransform::Identity, FActorSpawnParameters());

	Grid->Init(FVector2D(RoomWidthUnits,RoomHeightUnits));
		
	UImageProcessor* ImageProcessor = NewObject<UImageProcessor>();
	
	// First room is always going to be in the center of the grid
	FVector2D CurrentRoomCoord = Grid->GetCentralRoomCoord();
	Grid->AddRoom(CurrentRoomCoord);
	
	// Now add the remaining rooms
	for (uint8 Idx = 1; Idx < RoomNr; Idx++)
	{
		bool ValidRoomFound = false;
		FVector2D ChosenRoomCoord;
		
		while (!ValidRoomFound)
		{
			ChosenRoomCoord = SelectAdjacentRoomCoord(CurrentRoomCoord);
			ValidRoomFound = !Grid->IsRoom(ChosenRoomCoord);
		}
			CurrentRoomCoord = ChosenRoomCoord;
			Grid->AddRoom(ChosenRoomCoord);
	}

	for (int RoomIndex = 0; RoomIndex < Grid->RoomsCoordinates.Num(); RoomIndex++)
	{
		if (RoomIndex == 1)
		{
			// First room should be empty?	
		}
	
		if (RoomIndex == Rooms.Num() - 1)
		{
			// Last room should contain the exit		
		}
		
		const FVector2D RoomCoord = Grid->RoomsCoordinates[RoomIndex];
		TArray<CardinalPoint> AdjacentRoomsCardinalPoints;
		Grid->GetAdjacentRoomCardinalPoints(AdjacentRoomsCardinalPoints, RoomCoord);
	 	
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

			FVector2D CurrentWorldTile = FVector2D(RoomCoord.X * RoomWidthUnits, RoomCoord.Y * RoomWidthUnits) + PixelCoord;

			// Adding an offset of GridTileSize/2 because Actor mesh has pivot in the middle
			FVector SpawnLocation = FVector(CurrentWorldTile, 0) * Grid->GridTileSize + FVector(Grid->GridTileSize/2, Grid->GridTileSize/2, 0);
			FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation, FVector(Grid->GridTileSize / 100));

			bool bSpawnWall = false;

			if (Image[PixelIndex] == FVector(0,0,0))
			{
				bSpawnWall = true;
			}
			
			// Outer walls detection for room connection/sealing logic
			if (
				PixelCoord.X == 0 && !AdjacentRoomsCardinalPoints.Contains(West) ||
				PixelCoord.X == RoomWidthUnits - 1 && !AdjacentRoomsCardinalPoints.Contains(East) ||
				PixelCoord.Y == 0 && !AdjacentRoomsCardinalPoints.Contains(South) ||
				PixelCoord.Y == RoomHeightUnits - 1 && !AdjacentRoomsCardinalPoints.Contains(North)
				)
			{
				bSpawnWall = true;
			}

				if (bSpawnWall)
				{
					SpawnedWalls.Add(GetWorld()->SpawnActor(WallActor, &SpawnTransform, FActorSpawnParameters()));
					Grid->AddBlockedTile(CurrentWorldTile.IntPoint());
				}
					
		}
		// 	// -- WIP Test Image data --
		// 	int x = 2;
		// 	int y = 0;
		// 	size_t index = y * Width + x;
		// 	// UE_LOG(LogTemp, Warning, TEXT("RGB pixel @ (x=%d, y=%d): %s"), x, y, *Image[index].ToString());
		// 	// -------------------------
		// }
		
		if (GridNavigationData)
			GridNavigationData->Init(Grid);
	}
}

void ALevelSpawner::BeginPlay()
{
	Super::BeginPlay();
	const AActor* NavData = UGameplayStatics::GetActorOfClass(GetWorld(), AGridNavigationData::StaticClass());
	AGridNavigationData* GridNavData = (AGridNavigationData*)NavData;
	GridNavigationData = GridNavData;
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

