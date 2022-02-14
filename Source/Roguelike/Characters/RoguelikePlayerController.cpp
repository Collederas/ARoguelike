#include "RoguelikePlayerController.h"

void ARoguelikePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UGridMovementComponent* GridMovementComponent = (UGridMovementComponent*)InPawn->GetMovementComponent();
	GridMovementComponent->OnMoveComplete.AddUObject(this, &ARoguelikePlayerController::OnMoveCompleted);
}

void ARoguelikePlayerController::OnMoveCompleted(const FPathFollowingResult& Result)
{
	ReceiveMoveCompleted.Broadcast(Result.Code);
}
