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

FVector ARogueLikePawn::MoveForward()
{
	return MovementComponent->RequestMove(EMoveDirection::Forward);
}

FVector ARogueLikePawn::MoveBackward()
{
	return MovementComponent->RequestMove(EMoveDirection::Backward);
}

FVector ARogueLikePawn::MoveLeft()
{
	return MovementComponent->RequestMove(EMoveDirection::Left);
}

FVector ARogueLikePawn::MoveRight()
{
	return MovementComponent->RequestMove(EMoveDirection::Right);
}