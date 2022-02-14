#include "WorldGrid.h"

bool FGridActor::operator!=(const FGridActor& OtherGridActor) const
{
	if (OtherGridActor.Actor)
		return OtherGridActor.Actor != Actor || OtherGridActor.ActorType != ActorType;
	return OtherGridActor.ActorType != ActorType;
}

bool FGridActor::operator==(const FGridActor& OtherGridActor) const
{
	if (OtherGridActor.Actor)
		return OtherGridActor.Actor == Actor && OtherGridActor.ActorType != ActorType;
	return OtherGridActor.ActorType == ActorType;
}

FVector AGrid::GetWorldLocationForGridCell(const FVector2D& Pos) const
{
	return FVector(Pos.X * GridTileSize, Pos.Y * GridTileSize, 0);
}

bool AGrid::GetGridCellForWorldLocation(const FVector& WorldPos, FVector2D& GridPos) const
{
	GridPos.X = (WorldPos.X/ GridTileSize) - 0.5;
	GridPos.Y = (WorldPos.Y/ GridTileSize) - 0.5;
	return (GridPos.X >= 0 && GridPos.Y >= 0 && GridPos.X < GridSize.X && GridPos.Y < GridSize.Y);
}

FVector AGrid::GetWorldLocationForGridCellCentre(const FIntPoint& pos) const
{
	return GetWorldLocationForGridCell(pos) + (FVector(GridTileSize, GridTileSize, 0) * 0.5f);
}

FVector2D AGrid::GetRoomCoord(FVector WorldCoord)
{
	FVector2D GridCoord;
	GetGridCellForWorldLocation(WorldCoord, GridCoord);

	const int RoomX = FMath::Floor(GridCoord.IntPoint().X / RoomDimensions.X);
	const int RoomY = FMath::Floor(GridCoord.IntPoint().Y / RoomDimensions.Y);
	return FVector2D(RoomX, RoomY);
}

FVector2D AGrid::GetCentralRoomCoord() const
{
	return FVector2D(FMath::Floor((NumberOfRooms.X - 1)/2), FMath::Floor((NumberOfRooms.Y - 1)/2));
}

FVector2D AGrid::GetRandomPointInRoom(FVector2D RoomCoord)
{
	const FVector2D RoomOrigin = GetRoomOrigin(RoomCoord);
	
	const int RandomX = FMath::RandRange(RoomOrigin.X + 1, RoomOrigin.X + RoomDimensions.X - 1);
	const int RandomY = FMath::RandRange(RoomOrigin.Y + 1, RoomOrigin.Y + RoomDimensions.Y - 1);
	return FVector2D(RandomX, RandomY);
}

bool AGrid::AddRoom(FVector2D RoomCoord)
{
	if (IsRoom(RoomCoord))
		return false;
		
	const int RoomIndex = NumberOfRooms.X * RoomCoord.Y + RoomCoord.X;
	RoomTiles[RoomIndex] = 1;
	RoomsCoordinates.Add(RoomCoord);
	return true;
}

bool AGrid::IsRoom(const FVector2D Coord)
{
	int RoomIndex = NumberOfRooms.X * Coord.Y + Coord.X;
	if (RoomTiles.Num() < RoomIndex + 1)
		return false;
	return static_cast<bool>(RoomTiles[RoomIndex]);
}

void AGrid::GetAdjacentRoomCardinalPoints(TArray<CardinalPoint>& Result, const FVector2D Coord)
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

void AGrid::PrintRoomsLayoutDebug()
{
	FString Output;
	for (int Idx = 0; Idx < RoomTiles.Num(); Idx++)
	{
		if (Idx % GridSize.X == 0)
		{
			Output += FString("\n");
		}
		Output += FString::Printf(TEXT("%d, "), RoomTiles[Idx]);
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"), *Output);
}

bool AGrid::IsGridCellWalkable(const FIntPoint& Location) const
{
	return (!BlockedTiles.Contains(Location));
}

void AGrid::AddBlockedTile(FIntPoint Location)
{
	BlockedTiles.Add(Location);
	UpdateActorLocationMap(Location, FGridActor(Wall));
}

bool AGrid::IsValidGridCell(const FIntPoint& Location) const
{
	return (Location.X >= 0 && Location.Y >= 0 && Location.X < GridSize.X && Location.Y < GridSize.Y);
}

FVector2D AGrid::GetRoomOrigin(FVector2D RoomGridCoord)
{
	return FVector2D(RoomDimensions.X * RoomGridCoord.X, RoomDimensions.Y * RoomGridCoord.Y);
}

void AGrid::Empty()
{
	RoomTiles.Empty();
	RoomsCoordinates.Empty();
	BlockedTiles.Empty();
}

void AGrid::UpdateActorLocationMap(const FVector2D Tile, const FGridActor GridActor)
{
	ActorLocationMap.Add(Tile, GridActor);
}

bool AGrid::CheckGridOccupied(FVector2D Tile, FGridActor& GridActor)
{
	GridActor = ActorLocationMap.FindRef(Tile);
	if (GridActor.Actor == nullptr && GridActor.ActorType == None)
		return false;
	return true;
}	

bool AGrid::WorldPositionCheckGridOccupied(FVector WorldPosition, FGridActor& GridActor)
{
	FVector2D TileCoord;
	GetGridCellForWorldLocation(WorldPosition, TileCoord);
	return CheckGridOccupied(TileCoord, GridActor);
}


