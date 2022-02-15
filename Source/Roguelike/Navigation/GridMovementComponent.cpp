#include "GridMovementComponent.h"
#include "GameFramework/Character.h"

UGridMovementComponent::UGridMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

FVector UGridMovementComponent::RequestMove(EMoveDirection Direction)
{
	FVector DirectionVector;
	const FRotator Rotation = PawnOwner->Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	const FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	const FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	switch (Direction)
	{
		case EMoveDirection::Forward:
			DirectionVector = ForwardVector;
			break;
		case EMoveDirection::Backward:
			DirectionVector = -1 * ForwardVector;
			break;
		case EMoveDirection::Left:
			DirectionVector = -1 * RightVector;
			break;
		case EMoveDirection::Right:
			DirectionVector = RightVector;
			break;
		default:
			DirectionVector = FVector::ZeroVector;
	}

	RequestedMove = GetOwner()->GetActorLocation() + DirectionVector * GridUnitSize;
	MoveTimeElapsed = 0;
	return RequestedMove;
}

void UGridMovementComponent::StopMovementImmediately()
{
	RequestedMove = FVector::ZeroVector;
}

void UGridMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (MoveTimeElapsed < MoveDuration && RequestedMove != FVector::ZeroVector)
	{
		if (DeltaTime > 0)
		{
			GetOwner()->SetActorLocation(FMath::Lerp(GetOwner()->GetActorLocation(), RequestedMove, MoveTimeElapsed/MoveDuration));
			MoveTimeElapsed += DeltaTime;	
		}
	}
	else
	{
		if (RequestedMove != FVector::ZeroVector)
		{
			FPathFollowingResult MoveResult;
			MoveResult.Code = EPathFollowingResult::Success;
			OnMoveComplete.Broadcast(MoveResult);
		}
		RequestedMove = FVector::ZeroVector;
	}
}
