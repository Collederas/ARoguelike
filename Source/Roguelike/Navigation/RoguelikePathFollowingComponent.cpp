#include "RoguelikePathFollowingComponent.h"

#include "Roguelike/Characters/RoguelikeAIController.h"


void URoguelikePathFollowingComponent::SetMoveSegment(int32 SegmentStartIndex)
{
	Super::SetMoveSegment(SegmentStartIndex);

	// TODO: Can improve this by using worldtogrid transformation (which should remove the need to cast)
	const FVector2D CurrentDes2D = static_cast<FVector2D>(CurrentDestination.Position);
	if (CurrentDes2D.Equals(TargetLocation, 50))
	{
		ARoguelikeAIController* MyController = Cast<ARoguelikeAIController>(GetOwner());
		MyController->OnAboutToReachTarget();
	}
}

void URoguelikePathFollowingComponent::UpdateTargetLocation(FVector2D NewTargetLocation)
{
	TargetLocation = NewTargetLocation;
}
