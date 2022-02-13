#pragma once

#include "CoreMinimal.h"
#include "GraphAStar.h"
#include "GridPathfinding.h"
#include "NavigationData.h"
#include "Roguelike/WorldGrid.h"
#include "GridNavigationData.generated.h"

UCLASS(config = Engine, defaultconfig, hidecategories = (Input, Rendering, Tags, "Utilities|Transformation", Actor, Layers, Replication), notplaceable)
class ROGUELIKE_API AGridNavigationData : public ANavigationData
{
	GENERATED_BODY()

public:
	virtual void Init(AGrid* GridActor);

	static FPathFindingResult FindPath(const FNavAgentProperties& AgentProperties, const FPathFindingQuery& Query);
	virtual bool ProjectPoint(const FVector& Point, FNavLocation& OutLocation, const FVector& Extent, FSharedConstNavQueryFilter Filter = NULL, const UObject* Querier = NULL) const override;
	virtual bool GetRandomReachablePointInRadius(const FVector& Origin, float Radius, FNavLocation& OutResult, FSharedConstNavQueryFilter Filter, const UObject* Querier) const override;

	virtual bool GetRandomReachablePointInRoom(const FVector& Origin, FVector& OutLocation, const APawn* Querier);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AGrid*  WorldGridActor;

private:
	FGridGraph Graph;
	FGraphAStar<FGridGraph>* Pathfinder;
};


