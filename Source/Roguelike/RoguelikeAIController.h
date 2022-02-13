#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RoguelikeCharacter.h"
#include "WorldGrid.h"
#include "Navigation/GridNavigationData.h"
#include "RoguelikeAIController.generated.h"

UCLASS()
class ROGUELIKE_API ARoguelikeAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintPure, meta = (WorldContext="WorldContextObject"))
	static void RandomReachablePointInCurrentRoom(const UObject* WorldContextObject, APawn* QuerierPawn, FVector& RandomLocation, ANavigationData* NavData = nullptr);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnMoveCompleted_Blueprint(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	
protected:
	UPROPERTY(BlueprintReadOnly)
	AGridNavigationData* GridNavigationData;

	UFUNCTION(BlueprintCallable)
	virtual EPathFollowingRequestResult::Type GridMoveTo(FVector Dest, TSubclassOf<UNavigationQueryFilter> FilterClass, int NumberOfMoves);
	
	virtual FPathFollowingRequestResult GridMoveTo(FVector Dest, FAIMoveRequest MoveRequest, int NumberOfMoves, FNavPathSharedPtr* OutPath = nullptr);
};
