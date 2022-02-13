#include "GridNavigationData.h"
#include "AI/Navigation/NavigationTypes.h"
#include "../WorldGrid.h"
#include "Kismet/GameplayStatics.h"
#include "VisualLogger/VisualLogger.h"


void AGridNavigationData::Init(AGrid* GridActor)
{
	WorldGridActor = GridActor;
	FindPathImplementation = FindPath;
	FindHierarchicalPathImplementation = FindPath;
	Graph = FGridGraph(WorldGridActor);
	Pathfinder = new FGraphAStar<FGridGraph>(Graph);
}

bool AGridNavigationData::ProjectPoint(const FVector& Point, FNavLocation& OutLocation, const FVector& Extent, FSharedConstNavQueryFilter Filter, const UObject* Querier) const
{
	FVector2D GridPos;
	if (WorldGridActor->GetGridCellForWorldLocation(Point, GridPos))
	{
		OutLocation = FNavLocation(Point);
		return true;
	}

	return false;
}

bool AGridNavigationData::GetRandomReachablePointInRadius(const FVector& Origin, float Radius, FNavLocation& OutResult,
    FSharedConstNavQueryFilter Filter, const UObject* Querier) const
{
    int RandomOffset = FMath::RandRange(-(int)Radius, (int)Radius);
    if (RandomOffset == 0)
            RandomOffset = 1;

    const FVector DestinationWorldSpace = Origin + FVector(RandomOffset,RandomOffset, 0);

    FVector2D DestinationGridCell;
    WorldGridActor->GetGridCellForWorldLocation(DestinationWorldSpace, DestinationGridCell);

	if (!WorldGridActor->IsGridCellWalkable(DestinationGridCell.IntPoint()))
    {
            OutResult = FNavLocation(Origin, INVALID_NAVNODEREF);
            return false;
    }
    
    	OutResult = FNavLocation(DestinationWorldSpace, INVALID_NAVNODEREF);
    	return true;
}

bool AGridNavigationData::GetRandomReachablePointInRoom(const FVector& Origin,
	FVector& OutLocation, const APawn* Querier)
{
	bool bFoundValidLocation = false;
	while(!bFoundValidLocation)
	{
		const FVector2D RoomCoord = WorldGridActor->GetRoomCoord(Querier->GetActorLocation());

		const FVector2D RandomPointInRoom = WorldGridActor->GetRandomPointInRoom(RoomCoord);
	
		const FVector DestinationWorldSpace = WorldGridActor->GetWorldLocationForGridCell(RandomPointInRoom);
		FVector2D DestinationGridCell;
		WorldGridActor->GetGridCellForWorldLocation(DestinationWorldSpace, DestinationGridCell);
		
		if (WorldGridActor->IsGridCellWalkable(DestinationGridCell.IntPoint()))
		{
			OutLocation = DestinationWorldSpace;
			bFoundValidLocation = true;
		}
	}
	return true;
}

FPathFindingResult AGridNavigationData::FindPath(const FNavAgentProperties& AgentProperties, const FPathFindingQuery& Query)
{
	const ANavigationData* Self = Query.NavData.Get();
	const AGridNavigationData* GridNavData = static_cast<const AGridNavigationData*>(Self);
	check(GridNavData != nullptr);

	if (GridNavData == nullptr)
		return ENavigationQueryResult::Error;

	FPathFindingResult Result(ENavigationQueryResult::Error);
	Result.Path = Query.PathInstanceToFill.IsValid() ? Query.PathInstanceToFill : Self->CreatePathInstance<FNavigationPath>(Query);

	FNavigationPath* NavPath = Result.Path.Get();

	if (NavPath != nullptr)
	{
		if ((Query.StartLocation - Query.EndLocation).IsNearlyZero())
		{
			Result.Path->GetPathPoints().Reset();
			Result.Path->GetPathPoints().Add(FNavPathPoint(Query.EndLocation));
			Result.Result = ENavigationQueryResult::Success;
		}
		else if (Query.QueryFilter.IsValid())
		{
			FVector2D MyGridPos, TargetGridPos = FVector2D::ZeroVector;
			GridNavData->WorldGridActor->GetGridCellForWorldLocation(Query.StartLocation, MyGridPos);
			GridNavData->WorldGridActor->GetGridCellForWorldLocation(Query.EndLocation, TargetGridPos);

			GridGraphQueryFilter QueryFilter;
			QueryFilter.SetWorldGrid(GridNavData->WorldGridActor);
			QueryFilter.SetAllowPartialPaths(false);

			TArray<FIntPoint> Path;

			// UE_LOG(LogTemp, Warning, TEXT("Finding path from start grid: %s to end grid: %s"), *MyGridPos.IntPoint().ToString(), *TargetGridPos.IntPoint().ToString());

			const EGraphAStarResult AStarResult = GridNavData->Pathfinder->FindPath(MyGridPos.IntPoint(), TargetGridPos.IntPoint(), QueryFilter, Path);

			if (AStarResult == EGraphAStarResult::SearchFail || AStarResult == EGraphAStarResult::InfiniteLoop)
			{
				UE_LOG(LogTemp, Warning, TEXT("AGridNavigationData A* fail: %d"), AStarResult);

				Result.Result = ENavigationQueryResult::Fail;
				return Result;
			}

			Path.Insert(MyGridPos.IntPoint(), 0);

			for (auto& Point : Path)
			{
				NavPath->GetPathPoints().Add(FNavPathPoint(GridNavData->WorldGridActor->GetWorldLocationForGridCellCentre(Point)));
			}

			// UE_LOG(LogTemp, Warning, TEXT("WorldGridNav path (%d points):"), Path.Num());
			// for (int i = 0; i < Path.Num(); i++)
			// {
			//	   UE_LOG(LogTemp, Warning, TEXT("%s (%s)"), *Path[i].ToString(), *NavPath->GetPathPoints()[i].Location.ToString());
			// }
			
			NavPath->MarkReady();
			Result.Result = ENavigationQueryResult::Success;
		}
	}

	return Result;
}
