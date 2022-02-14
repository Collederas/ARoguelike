#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Roguelike/Characters/RoguelikeCharacter.h"
#include "Roguelike/WorldGrid.h"
#include "Roguelike/Navigation/GridNavigationData.h"
#include "Roguelike/Navigation/RoguelikePathFollowingComponent.h"
#include "RoguelikeAIController.generated.h"

UCLASS()
class ROGUELIKE_API ARoguelikeAIController : public AAIController
{
	GENERATED_BODY()

public:
	ARoguelikeAIController(const FObjectInitializer& Initializer = FObjectInitializer::Get());
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	URoguelikePathFollowingComponent* GetRLPathFollowingComponent();
	
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

private:
	UPROPERTY()
	URoguelikePathFollowingComponent* RLPathFollowingComponent;
};
