#pragma once

#include "CoreMinimal.h"
#include "../WorldGrid.h"

class ROGUELIKE_API GridGraphQueryFilter
{
public:
    GridGraphQueryFilter()
        : WorldGrid(nullptr)
        , AllowPartialPaths(false)
    {}

    void SetWorldGrid(AGrid* InWorldGrid)
    {
        WorldGrid = InWorldGrid;
    }

    float GetHeuristicScale() const
    {
        return 1;
    }

    float GetHeuristicCost(const FIntPoint StartNodeRef, const FIntPoint EndNodeRef) const
    {
        return FMath::Abs(StartNodeRef.X - EndNodeRef.X) + FMath::Abs(StartNodeRef.Y - EndNodeRef.Y);
    }

    float GetTraversalCost(const FIntPoint StartNodeRef, const FIntPoint EndNodeRef) const
    {
        return 1;
    }

    bool IsTraversalAllowed(const FIntPoint NodeA, const FIntPoint NodeB) const
    {
        if (!WorldGrid->IsGridCellWalkable(NodeA) || !WorldGrid->IsGridCellWalkable(NodeB))
            return false;
        return true;
    }

    bool WantsPartialSolution() const
    {
        return AllowPartialPaths;
    }

    void SetAllowPartialPaths(const bool Allow)
    {
        AllowPartialPaths = Allow;
    }

private:
    AGrid* WorldGrid;
    bool AllowPartialPaths;
};

struct ROGUELIKE_API FGridGraph
{
    FGridGraph() {};
    
    FGridGraph(AGrid* InWorldGrid)
        : WorldGrid(InWorldGrid)
    {
    }

    FORCEINLINE AGrid* GetGrid() const { return WorldGrid; }

    typedef FIntPoint FNodeRef;

    static int32 GetNeighbourCount(FNodeRef NodeRef);
    bool IsValidRef(FNodeRef NodeRef) const;
    FNodeRef GetNeighbour(const FNodeRef NodeRef, const int32 NeighbourIndex) const;

private:
    AGrid* WorldGrid;
};
