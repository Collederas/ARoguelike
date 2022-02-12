#include "RoguelikeAIController.h"

#include "Kismet/GameplayStatics.h"
#include "Navigation/GridNavigationData.h"

void ARoguelikeAIController::BeginPlay()
{
	Super::BeginPlay();
	AGridNavigationData* WorldGridNavigationData = (AGridNavigationData*)UGameplayStatics::GetActorOfClass(GetWorld(), AGridNavigationData::StaticClass());
	check(WorldGridNavigationData);

	GridNavigationData = WorldGridNavigationData;
}

void ARoguelikeAIController::RandomReachablePointInCurrentRoom(FVector& RandomLocation)
{
	GridNavigationData->GetRandomReachablePointInRoom(GetPawn()->GetActorLocation(), GetRoom(GetPawn()), RandomLocation, this);
}

FVector2D ARoguelikeAIController::GetRoom(AActor* Actor)
{
	return GridNavigationData->WorldGridActor->GetRoomCoord(GetPawn()->GetActorLocation());
}
