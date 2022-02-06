#pragma once

#include "CoreMinimal.h"
#include "Roguelike.generated.h"


UCLASS()
class URoom: public UObject
{
	GENERATED_BODY()
	
	FVector2D Coord;
	bool operator==(const URoom& Other) const;
};

UCLASS()
class UGrid : public UObject
{
	GENERATED_BODY()
	
	TArray<URoom> Rooms;
};