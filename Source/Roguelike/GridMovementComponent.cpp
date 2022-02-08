#include "GridMovementComponent.h"
#include "GameFramework/Character.h"

UGridMovementComponent::UGridMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGridMovementComponent::AddInputVector(FVector WorldVector, bool bForce)
{
	Super::AddInputVector(WorldVector, bForce);
}

void UGridMovementComponent::Move(EMoveDirection Direction)
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

	GetOwner()->SetActorLocation(GetOwner()->GetActorLocation() + DirectionVector * GridUnitSize);
}

void UGridMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}
