#pragma once

#include "CoreMinimal.h"
#include "WorldGrid.h"
#include "GameFramework/Actor.h"
#include "Navigation/GridNavigationData.h"
#include "LevelSpawner.generated.h"

USTRUCT(BlueprintType)
struct FSpawnActorConfig
{
	GENERATED_BODY()
	FSpawnActorConfig(): ActorType(None)
	{
	}

	FSpawnActorConfig(TSubclassOf<AActor> InActorClass,
	                  TEnumAsByte<EGridActorType> InActorType): ActorClass(InActorClass), ActorType(InActorType)
	{
	}

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EGridActorType> ActorType;

	UPROPERTY(EditAnywhere)
	FVector SpawnOffset = FVector::ZeroVector;
};

UCLASS(Config=Game)
class ROGUELIKE_API ALevelSpawner : public AActor
{
	GENERATED_BODY()

public:
	ALevelSpawner();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int BaseRoomNr = 5;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int RoomHeightUnits = 16;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int RoomWidthUnits = 16;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (RelativeToGameContentDir))
	TArray<FFilePath> RoomSourceImages;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (RelativeToGameContentDir))
	TArray<FFilePath> StartRoomImages;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (RelativeToGameContentDir))
	TArray<FFilePath> ExitRoomImages;
	
	// WARNING: Color Alpha should always be 255
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<FColor, FSpawnActorConfig> ColorActorMap;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> WallActor;

	UFUNCTION(BlueprintCallable)
	virtual void SpawnNewLevel(int NrOfRooms, bool SpawnChest);

	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetAllEnemies();

	UFUNCTION()
	FVector2D GetTileCoordFromRoom(FVector2D RoomCoord) const;

	UFUNCTION(BlueprintCallable)
	void GetStartingPointTransform(FTransform& Transform) const;

	UFUNCTION(BlueprintCallable)
	FVector GetDungeonCentralLocation();

protected:
	UPROPERTY(Transient)
	TObjectPtr<AGrid> Grid;

	UPROPERTY()
	TArray<FGridActor> SpawnedActors;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Contain the coordinates of all level room on the main grid
	TArray<FVector2D> Rooms;

	// Return Grid coordinate of a random adjacent room
	virtual FVector2D SelectAdjacentRoomCoord(const FVector2D RoomCoord);

	virtual FString GetRandomSourceImage(TArray<FFilePath> Sources);

	virtual void SpawnActorAtLocation(FVector SpawnLocation, TSubclassOf<AActor> ActorClass, FGridActor& GridActor, bool SetBlockTile = false);
private:
	TObjectPtr<AGridNavigationData> GridNavigationData;
	bool bInitializedGridNav;

	TMap<int, float> RandomRotatorYawMap;
};
