#include "RoguelikePathFollowingComponent.h"


void URoguelikePathFollowingComponent::OnSegmentFinished()
{
	Super::OnSegmentFinished();
	const FNavigationPath* PathInstance = Path.Get();
	
	UE_LOG(LogTemp, Warning, TEXT("NextPathLocation %s, TargetLocation: %s"), *PathInstance->GetPathPoints()[GetNextPathIndex() + 1].Location.ToString(), *TargetLocation.ToString());
	FVector2D ActorLocation2D = (FVector2D)MovementComp->GetActorLocation();
	if (ActorLocation2D.Equals(TargetLocation, 199))
	{
		UE_LOG(LogTemp, Warning, TEXT("Next move is GAME OVER >:)"), *CurrentDestination.Position.ToString());
	}
}


void URoguelikePathFollowingComponent::UpdateTargetLocation(FVector2D NewTargetLocation)
{
	TargetLocation = NewTargetLocation;
}
