#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelSpawner.generated.h"

UCLASS()
class ROGUELIKE_API ALevelSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelSpawner();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RoomNr = 5;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float UnitScale = 100;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float GridSizeUnits = 10; 
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RoomHeightUnits = 16;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RoomWidthUnits = 16;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Contain the coordinates of all level room on the main grid
	TArray<FVector2D> RoomsCoord;

	// Return Grid coordinate of a random adjacent room
	virtual FVector2D SelectAdjacentRoomCoord(const FVector2D RoomCoord);
};
