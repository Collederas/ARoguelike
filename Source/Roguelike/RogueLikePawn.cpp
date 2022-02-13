#include "RogueLikePawn.h"

ARogueLikePawn::ARogueLikePawn()
{
	PrimaryActorTick.bCanEverTick = true;
	MovementComponent = CreateDefaultSubobject<UGridMovementComponent>(TEXT("GridMovementComponent"));
}

UGridMovementComponent* ARogueLikePawn::GetMovementComponent() const
{
	return MovementComponent;
}

void ARogueLikePawn::MoveForward()
{
	MovementComponent->RequestMove(EMoveDirection::Forward);
}

void ARogueLikePawn::MoveBackward()
{
	MovementComponent->RequestMove(EMoveDirection::Backward);
}

void ARogueLikePawn::MoveLeft()
{
	MovementComponent->RequestMove(EMoveDirection::Left);
}

void ARogueLikePawn::MoveRight()
{
	MovementComponent->RequestMove(EMoveDirection::Right);
}