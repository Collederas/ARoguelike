#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"
#include "RoguelikePathFollowingComponent.generated.h"

UCLASS()
class ROGUELIKE_API URoguelikePathFollowingComponent : public UPathFollowingComponent
{
	GENERATED_BODY()

	virtual void OnSegmentFinished() override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void UpdateTargetLocation(FVector2D NewTargetLocation);

protected:
	// Used to keep track of the target and notify whenever the current
	// path passes through it.
	UPROPERTY()
	FVector2D TargetLocation = FVector2D::ZeroVector;
};
