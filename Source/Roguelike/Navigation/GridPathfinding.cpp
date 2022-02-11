#include "GridPathfinding.h"

int32 FGridGraph::GetNeighbourCount(FNodeRef NodeRef)
{
	return 4;
}

bool FGridGraph::IsValidRef(FNodeRef NodeRef) const
{
	return WorldGrid->IsValidGridCell(NodeRef);
}

FGridGraph::FNodeRef FGridGraph::GetNeighbour(const FNodeRef NodeRef, const int32 NeighbourIndex) const
{
	static const FIntPoint NeighbourOffset[4] =
	{
		{ 0, 1 },
		{ 1, 0 },
		{ 0, -1 },
		{ -1, 0 },
	};

	return NodeRef + NeighbourOffset[NeighbourIndex];
}