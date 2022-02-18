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

	// Constrain to closest Cardinal direction
	float AbsRVX = FMath::Abs(RightVector.X);
	float AbsRVY = FMath::Abs(RightVector.Y);
	float MaxRightComponent = FMath::Max(AbsRVX, AbsRVY);
	FVector CardinalRight;
	if (MaxRightComponent == AbsRVX)
	{
		float OriginalSign = RightVector.X > 0 ? 1 : -1;
		CardinalRight = FVector(1, 0, 0) * OriginalSign;
	}
	else
	{
		float OriginalSign = RightVector.Y > 0 ? 1 : -1;
		CardinalRight = FVector(0, 1, 0) * OriginalSign;
	}

	float AbsFVX = FMath::Abs(ForwardVector.X);
	float AbsFVY = FMath::Abs(ForwardVector.Y);
	float MaxForwardComponent = FMath::Max(AbsFVX, AbsFVY);
	FVector CardinalForward;
	if (MaxForwardComponent == AbsFVX)
	{
		float OriginalSign = ForwardVector.X > 0 ? 1 : -1;
		CardinalForward = FVector(1, 0, 0) * OriginalSign;
	}
	else
	{
		float OriginalSign = ForwardVector.Y > 0 ? 1 : -1;
		CardinalForward = FVector(0, 1, 0) * OriginalSign;
	}
	//
	
	switch (Direction)
	{
		case EMoveDirection::Forward:
			DirectionVector = CardinalForward;
			break;
		case EMoveDirection::Backward:
			DirectionVector = -1 * CardinalForward;
			break;
		case EMoveDirection::Left:
			DirectionVector = -1 * CardinalRight;
			break;
		case EMoveDirection::Right:
			DirectionVector = CardinalRight;
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
