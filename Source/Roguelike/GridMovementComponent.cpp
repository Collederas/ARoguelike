#include "GridMovementComponent.h"
#include "GameFramework/Character.h"

UGridMovementComponent::UGridMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGridMovementComponent::AddInputVector(FVector WorldVector, bool bForce)
{
	Super::AddInputVector(WorldVector, bForce);
}

void UGridMovementComponent::RequestMove(EMoveDirection Direction)
{
	FVector DirectionVector;
	switch (Direction)
	{
		case EMoveDirection::Forward:
			DirectionVector = FVector::ForwardVector;
			break;
		case EMoveDirection::Backward:
			DirectionVector = -1 * FVector::ForwardVector;
			break;
		case EMoveDirection::Left:
			DirectionVector = -1 * FVector::RightVector;
			break;
		case EMoveDirection::Right:
			DirectionVector = FVector::RightVector;
			break;
		default:
			DirectionVector = FVector::ZeroVector;
	}

	RequestedMove = GetOwner()->GetActorLocation() + DirectionVector * GridUnitSize;
	MoveTimeElapsed = 0;
}

void UGridMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (WaitTime > 0)
	{
		WaitTime -= DeltaTime;
		return;
	}
	
	if (MoveTimeElapsed < MoveDuration && RequestedMove != FVector::ZeroVector)
	{
		GetOwner()->SetActorLocation(FMath::Lerp(GetOwner()->GetActorLocation(), RequestedMove, MoveTimeElapsed/MoveDuration));
		MoveTimeElapsed += DeltaTime;
	}
	else
	{
		RequestedMove = FVector::ZeroVector;
	}
}
