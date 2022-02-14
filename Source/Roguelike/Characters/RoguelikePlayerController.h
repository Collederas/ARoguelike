#pragma once

#include "CoreMinimal.h"
#include "Roguelike/Navigation/GridMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "RoguelikePlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGridMovementCompleteSignature, EPathFollowingResult::Type, Result);

UCLASS()
class ROGUELIKE_API ARoguelikePlayerController : public APlayerController
{
	GENERATED_BODY()
	ARoguelikePlayerController(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer) {}

	virtual void OnPossess(APawn* InPawn) override;

public:
	virtual void OnMoveCompleted(const FPathFollowingResult& Result);

	/** Blueprint notification that we've completed the current movement request */
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "MoveCompleted"))
	FGridMovementCompleteSignature ReceiveMoveCompleted;
};
