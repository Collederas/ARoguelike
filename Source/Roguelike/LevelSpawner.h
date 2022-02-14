#pragma once

#include "CoreMinimal.h"
#include "WorldGrid.h"
#include "GameFramework/Actor.h"
#include "Navigation/GridNavigationData.h"
#include "LevelSpawner.generated.h"

UCLASS(Config=Game)
class ROGUELIKE_API ALevelSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelSpawner();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int RoomNr = 5;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int RoomHeightUnits = 16;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int RoomWidthUnits = 16;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (RelativeToGameContentDir))
	TArray<FFilePath> RoomSourceImages;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<FColor, TSubclassOf<AActor>> ColorActorMap;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> WallActor;

	UFUNCTION(BlueprintCallable)
	virtual void SpawnNewLevel();

	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetAllEnemies();
	
protected:
	UPROPERTY(Transient)
	TObjectPtr<AGrid> Grid;

	UPROPERTY()
	TArray<AActor*> SpawnedWalls;
	
	UPROPERTY()
	TArray<AActor*> SpawnedEnemies;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Contain the coordinates of all level room on the main grid
	TArray<FVector2D> Rooms;

	// Return Grid coordinate of a random adjacent room
	virtual FVector2D SelectAdjacentRoomCoord(const FVector2D RoomCoord);
	
	virtual FString GetRandomSourceImage();
private:
	TObjectPtr<AGridNavigationData> GridNavigationData;
};


