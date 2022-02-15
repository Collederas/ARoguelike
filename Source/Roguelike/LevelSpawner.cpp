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

	UE_LOG(LogTemp, Log, TEXT("Spawning new Level with %d rooms"), RoomNr);

	for (auto SpawnedActor : SpawnedActors)
	{
		SpawnedActor.Actor->Destroy();
	}
	
	SpawnedActors.Empty();

	if (!Grid)
		Grid = (AGrid*)GetWorld()->SpawnActor(AGrid::StaticClass(), &FTransform::Identity, FActorSpawnParameters());

	Grid->Init(FVector2D(RoomWidthUnits, RoomHeightUnits));

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
		TArray<ECardinalPoint> AdjacentRoomsCardinalPoints;
		Grid->GetAdjacentRoomCardinalPoints(AdjacentRoomsCardinalPoints, RoomCoord);

		const FString SourceImagePath = GetRandomSourceImage();
		TArray<FVector> Image;
		int Width, Height;
		ImageProcessor->LoadImage(Image, Width, Height, SourceImagePath);

		FVector2D PixelCoord = FVector2D::ZeroVector;
		for (int PixelIndex = 0; PixelIndex < Image.Num(); PixelIndex++)
		{
			FColor CurrentColor = FColor(Image[PixelIndex].X, Image[PixelIndex].Y, Image[PixelIndex].Z);
			PixelCoord.X = PixelIndex % RoomWidthUnits;

			// Every time we wrap around, increase Pixel Y coord
			if (PixelCoord.X == 0 && PixelIndex > 0)
				PixelCoord.Y ++;
			// This means that this coordinate starts at 0 (eg.: on a 16x16px image, the max Y is 15)

			FVector2D CurrentWorldTile = FVector2D(RoomCoord.X * RoomWidthUnits, RoomCoord.Y * RoomWidthUnits) +
				PixelCoord;

			// Adding an offset of GridTileSize/2 because Actor mesh has pivot in the middle
			FVector SpawnLocation = FVector(CurrentWorldTile, 0) * Grid->GridTileSize + FVector(
				Grid->GridTileSize / 2, Grid->GridTileSize / 2, 0);
			FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation, FVector(Grid->GridTileSize / 100));

			bool bSpawnOuterWall = false;
			
			// Perimetral/Outer walls needs to be spawned independently of source image if
			// room is not bordering with anything on either side
			if (
				PixelCoord.X == 0 && !AdjacentRoomsCardinalPoints.Contains(West) ||
				PixelCoord.X == RoomWidthUnits - 1 && !AdjacentRoomsCardinalPoints.Contains(East) ||
				PixelCoord.Y == 0 && !AdjacentRoomsCardinalPoints.Contains(South) ||
				PixelCoord.Y == RoomHeightUnits - 1 && !AdjacentRoomsCardinalPoints.Contains(North)
			)
			{
				bSpawnOuterWall = true;
			}
			
			FSpawnActorConfig SpawnActorConfig = ColorActorMap.FindRef(CurrentColor);
			if (!bSpawnOuterWall && SpawnActorConfig.ActorType != None)
			{
				FVector CustomSpawnLoc = SpawnLocation + SpawnActorConfig.SpawnOffset;
				FGridActor GridActor = FGridActor(SpawnActorConfig.ActorType);
				
				switch (SpawnActorConfig.ActorType)
				{
				case Enemy:
					{
						APawn* const Enemy = (APawn*)GetWorld()->SpawnActor(SpawnActorConfig.ActorClass, &CustomSpawnLoc,
																			NULL, FActorSpawnParameters());
						GridActor.Actor = Enemy;
						SpawnedActors.Add(GridActor);
						Grid->UpdateActorLocationMap(CurrentWorldTile, GridActor);
						Enemy->SpawnDefaultController();
						break;
					}
				case Wall:
					Grid->AddBlockedTile(CurrentWorldTile.IntPoint());
					GridActor.Actor = GetWorld()->SpawnActor(SpawnActorConfig.ActorClass, &CustomSpawnLoc, NULL,
																	 FActorSpawnParameters());
					Grid->UpdateActorLocationMap(CurrentWorldTile, GridActor);
				default:
					{
						GridActor.Actor = GetWorld()->SpawnActor(SpawnActorConfig.ActorClass, &CustomSpawnLoc, NULL,
																		 FActorSpawnParameters());
						SpawnedActors.Add(GridActor);
						Grid->UpdateActorLocationMap(CurrentWorldTile, GridActor);

					}
				}
			}

			if (bSpawnOuterWall)
			{
				TArray<FSpawnActorConfig> ActorConfigs;
				ColorActorMap.GenerateValueArray(ActorConfigs);
				for (auto ActorConfig : ActorConfigs)
				{
					if (ActorConfig.ActorType == Wall)
					{
						FGridActor Wall = FGridActor(EGridActorType::Wall);
						Wall.Actor = GetWorld()->SpawnActor(WallActor, &SpawnTransform, FActorSpawnParameters());
						Grid->UpdateActorLocationMap(CurrentWorldTile.IntPoint(), Wall);
						Grid->AddBlockedTile(CurrentWorldTile.IntPoint());
					}
				}
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

	if (GridNavigationData)
		GridNavigationData->Init(Grid);
}

TArray<AActor*> ALevelSpawner::GetAllEnemies()
{
	TArray<AActor*> Result;
	for (auto SpawnedActor : SpawnedActors)
	{
		if (SpawnedActor.ActorType == Enemy)
			Result.Add(SpawnedActor.Actor);
	};
	return Result;
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
