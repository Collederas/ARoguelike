#include "LevelSpawner.h"
#include "ImageProcessor.h"
#include "WorldGrid.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/GridNavigationData.h"

ALevelSpawner::ALevelSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	RandomRotatorYawMap.Add(1, 90);
	RandomRotatorYawMap.Add(2, 180);
	RandomRotatorYawMap.Add(3, 270);
	RandomRotatorYawMap.Add(4, 0);
}

void ALevelSpawner::SpawnNewLevel(int NrOfRooms, bool SpawnChest)
{
	constexpr int MaxRoomNr = 20;

	if (NrOfRooms > MaxRoomNr)
	{
		// If linking room number to progression, maybe this could throw event for beating the game.
		UE_LOG(LogTemp, Warning, TEXT("Cannot spawn more than %d rooms. Aborting"), MaxRoomNr);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Spawning new Level with %d rooms"), NrOfRooms);

	for (auto SpawnedActor : SpawnedActors)
	{
		if (IsValid(SpawnedActor.Actor))
			SpawnedActor.Actor->Destroy();
	}

	SpawnedActors.Empty();

	if (!Grid)
		Grid = (AGrid*)GetWorld()->SpawnActor(AGrid::StaticClass(), &FTransform::Identity, FActorSpawnParameters());

	Grid->Init(FVector2D(RoomWidthUnits, RoomHeightUnits));

	UImageProcessor* ImageProcessor = NewObject<UImageProcessor>();

	// First room is always going to be in the center of the grid
	FVector2D CurrentRoomCoord = Grid->GetCentralRoomCoord();
	UE_LOG(LogTemp, Warning, TEXT("StrtingRoomCoord: %s"), *CurrentRoomCoord.ToString());
	Grid->AddRoom(CurrentRoomCoord);

	// Now add the remaining rooms
	for (uint8 Idx = 1; Idx < NrOfRooms; Idx++)
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

	bool bChestSpawned = false; // Keep track of this because there must be only 1 per level

	for (int RoomIndex = 0; RoomIndex < Grid->RoomsCoordinates.Num(); RoomIndex++)
	{
		const FVector2D RoomCoord = Grid->RoomsCoordinates[RoomIndex];
		TArray<ECardinalPoint> AdjacentRoomsCardinalPoints;
		Grid->GetAdjacentRoomCardinalPoints(AdjacentRoomsCardinalPoints, RoomCoord);

		FString SourceImagePath;

		if (RoomIndex == 0)
		{
			if (!StartRoomImages.IsEmpty())
				SourceImagePath = GetRandomSourceImage(StartRoomImages);
		}
		else if (RoomIndex == Grid->RoomsCoordinates.Num() - 1)
		{
			if (!ExitRoomImages.IsEmpty())
				SourceImagePath = GetRandomSourceImage(ExitRoomImages);
		}
		else
		{
			SourceImagePath = GetRandomSourceImage(RoomSourceImages);
		}

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

			// UE_LOG(LogTemp, Warning, TEXT("%s"), x, y, *Image[index].ToString());
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
						int RandomInt = FMath::RandRange(1, 4);
						FRotator SpawnRotation = FRotator(0, RandomRotatorYawMap.FindRef(RandomInt), 0);
						APawn* const Enemy = (APawn*)GetWorld()->SpawnActor(
							SpawnActorConfig.ActorClass, &CustomSpawnLoc,
							&SpawnRotation, FActorSpawnParameters());
						GridActor.Actor = Enemy;
						SpawnedActors.Add(GridActor);
						Grid->AddActorToLocationMap(CurrentWorldTile, GridActor);
						Enemy->SpawnDefaultController();
						break;
					}
				case Wall:
					{
						SpawnActorAtLocation(CustomSpawnLoc, SpawnActorConfig.ActorClass, GridActor, true);
						break;
					}
				case Chest:
					{
						if (!bChestSpawned && SpawnChest)
						{
							SpawnActorAtLocation(CustomSpawnLoc, SpawnActorConfig.ActorClass, GridActor, true);
							bChestSpawned = true;
						}
						break;
					}
				default:
					{
						SpawnActorAtLocation(CustomSpawnLoc, SpawnActorConfig.ActorClass, GridActor, false);
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
						Grid->AddActorToLocationMap(CurrentWorldTile.IntPoint(), Wall);
						Grid->AddBlockedTile(CurrentWorldTile.IntPoint());
						SpawnedActors.Add(Wall);
					}
				}
			}
		}
	}

	if (!bChestSpawned && SpawnChest)
	{
		int RandRoomIndex = FMath::RandRange(1, Grid->RoomsCoordinates.Num() - 1);
		FVector2D RandomRoomCoord = Grid->RoomsCoordinates[RandRoomIndex];
		FVector2D RandomTile = Grid->GetRandomFreeTileInRoom(RandomRoomCoord);

		TArray<FSpawnActorConfig> ActorConfigs;
		ColorActorMap.GenerateValueArray(ActorConfigs);
		for (auto ActorConfig : ActorConfigs)
		{
			if (ActorConfig.ActorType == Chest)
			{
				FVector SpawnLocation = Grid->GetWorldLocationForGridCell(RandomTile) + ActorConfig.SpawnOffset +
					FVector(Grid->GridTileSize / 2, Grid->GridTileSize / 2, 0);
				FGridActor ChestGridActor = FGridActor(EGridActorType::Chest);
				SpawnActorAtLocation(SpawnLocation, ActorConfig.ActorClass, ChestGridActor, true);
			}
		}
	}

	if (GridNavigationData && !bInitializedGridNav)
	{
		GridNavigationData->Init(Grid);
		bInitializedGridNav = true;
	}
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

FVector2D ALevelSpawner::GetTileCoordFromRoom(FVector2D RoomCoord) const
{
	return FVector2D(RoomCoord.X * RoomWidthUnits, RoomCoord.Y * RoomWidthUnits);
}

void ALevelSpawner::GetStartingPointTransform(FTransform& Transform) const
{
	if (!Grid)
		return;
	
	FVector2D StartingRoomOrigin;
	if (Grid->RoomsCoordinates.Num() > 0)
	{
		StartingRoomOrigin = Grid->GetCentralRoomCoord();
	}
	else
	{
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "GetStartingPointTransform - Attempt to access starting room when it was not yet created."
		       ));
		return;
	}
	if (Grid->IsRoom(StartingRoomOrigin))
	{
		FVector2D StartingTile = GetTileCoordFromRoom(StartingRoomOrigin) + FVector2D(
				FMath::RoundToInt(RoomWidthUnits / 2), FMath::RoundToInt(RoomHeightUnits / 2));
		
		UE_LOG(LogTemp, Warning, TEXT("StartingTile Found at: %s"), *StartingTile.ToString());
		
		const FVector StartingLocation = Grid->GetWorldLocationForGridCell(
			FVector2D(GetTileCoordFromRoom(StartingRoomOrigin) + FVector2D(
				FMath::RoundToInt(RoomWidthUnits / 2), FMath::RoundToInt(RoomHeightUnits / 2))));
		Transform = FTransform(FRotator::ZeroRotator, StartingLocation, FVector::OneVector);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GetStartingPointTransform - No room at central coords"));
	}
}

void ALevelSpawner::SpawnActorAtLocation(FVector SpawnLocation, TSubclassOf<AActor> ActorClass, FGridActor& GridActor,
                                         bool SetBlockTile)
{
	GridActor.Actor = GetWorld()->SpawnActor(ActorClass, &SpawnLocation, NULL,
	                                         FActorSpawnParameters());
	SpawnedActors.Add(GridActor);
	if (SetBlockTile)
		Grid->AddBlockedTileFromWorldLocation(SpawnLocation);

	Grid->AddActorToLocationMapWorldSpace(SpawnLocation, GridActor);
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

FString ALevelSpawner::GetRandomSourceImage(TArray<FFilePath> Sources)
{
	const int RandomIndex = FMath::FRandRange(0, Sources.Num() - 1);
	return Sources[RandomIndex].FilePath;
}
