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

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int RoomNr = 5;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float UnitScale = 100;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int GridSizeUnits = 10; 
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int RoomHeightUnits = 16;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int RoomWidthUnits = 16;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (RelativeToGameContentDir))
	TArray<FFilePath> RoomSourceImages;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Contain the coordinates of all level room on the main grid
	TArray<FVector2D> Rooms;

	// Return Grid coordinate of a random adjacent room
	virtual FVector2D SelectAdjacentRoomCoord(const FVector2D RoomCoord);
	
	virtual FString GetRandomSourceImage();

	// Gives the coordinate of the origin which by convention is in the top-left corner
	// (to map to source image origin)
	virtual FVector2D GetRoomOriginWorldSpace(FVector2D RoomCoords);
};


