#include "RoguelikeAIController.h"

#include "Kismet/GameplayStatics.h"
#include "Roguelike/Navigation/GridNavigationData.h"
#include "Roguelike/Navigation/RoguelikePathFollowingComponent.h"

ARoguelikeAIController::ARoguelikeAIController(const FObjectInitializer& Initializer) : Super(
	Initializer.SetDefaultSubobjectClass<URoguelikePathFollowingComponent>(TEXT("PathFollowingComponent")))
{
	GridActorOwner = FGridActor(GetOwner(), EGridActorType::Enemy);
}

void ARoguelikeAIController::BeginPlay()
{
	Super::BeginPlay();
	AGridNavigationData* WorldGridNavigationData = (AGridNavigationData*)UGameplayStatics::GetActorOfClass(
		GetWorld(), AGridNavigationData::StaticClass());
	check(WorldGridNavigationData);

	GridNavigationData = WorldGridNavigationData;
	RLPathFollowingComponent = (URoguelikePathFollowingComponent*)GetPathFollowingComponent();
}

URoguelikePathFollowingComponent* ARoguelikeAIController::GetRLPathFollowingComponent()
{
	return RLPathFollowingComponent;
}

void ARoguelikeAIController::RandomReachablePointInCurrentRoom(const UObject* WorldContextObject,
                                                               APawn* QuerierPawn,
                                                               FVector& RandomLocation,
                                                               ANavigationData* NavData)
{
	AGridNavigationData* GridNavigationData = (AGridNavigationData*)NavData;

	if (!GridNavigationData)
		GridNavigationData = (AGridNavigationData*)UGameplayStatics::GetActorOfClass(
			WorldContextObject, AGridNavigationData::StaticClass());

	GridNavigationData->GetRandomReachablePointInRoom(QuerierPawn->GetActorLocation(), RandomLocation, QuerierPawn);
}

void ARoguelikeAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	OnMoveCompleted_Blueprint(RequestID, Result.Code);
}

EPathFollowingRequestResult::Type ARoguelikeAIController::GridMoveTo(FVector Dest,
                                                                     TSubclassOf<UNavigationQueryFilter> FilterClass,
                                                                     int NumberOfMoves)
{
	FAIMoveRequest MoveReq(Dest);
	MoveReq.SetUsePathfinding(true);
	MoveReq.SetAllowPartialPath(false);
	MoveReq.SetProjectGoalLocation(false);
	MoveReq.SetNavigationFilter(*FilterClass ? FilterClass : DefaultNavigationFilterClass);
	MoveReq.SetAcceptanceRadius(-1.f);
	MoveReq.SetReachTestIncludesAgentRadius(false);
	MoveReq.SetCanStrafe(true);

	return GridMoveTo(Dest, MoveReq, NumberOfMoves);
}

FPathFollowingRequestResult ARoguelikeAIController::GridMoveTo(FVector Dest, FAIMoveRequest MoveRequest,
                                                               int NumberOfMoves, FNavPathSharedPtr* OutPath)
{
	FPathFollowingRequestResult ResultData;
	ResultData.Code = EPathFollowingRequestResult::Failed;

	UPathFollowingComponent* MyPathFollowingComponent = GetPathFollowingComponent();
	if (MyPathFollowingComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("MoveTo request failed due missing PathFollowingComponent"));
		return ResultData;
	}

	if (MoveRequest.IsValid() == false)
	{
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "MoveTo request failed due MoveRequest not being valid. Most probably desireg Goal Actor not longer exists"
		       ), *MoveRequest.ToString());
		return ResultData;
	}
	ensure(MoveRequest.GetNavigationFilter() || !DefaultNavigationFilterClass);

	if (MyPathFollowingComponent->HasReached(MoveRequest))
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveTo: already at goal!"));
		ResultData.MoveId = MyPathFollowingComponent->RequestMoveWithImmediateFinish(EPathFollowingResult::Success);
		ResultData.Code = EPathFollowingRequestResult::AlreadyAtGoal;
	}
	else
	{
		FPathFindingQuery PFQuery;
		const bool bValidQuery = BuildPathfindingQuery(MoveRequest, PFQuery);

		if (bValidQuery)
		{
			FNavPathSharedPtr PathResult;
			FindPathForMoveRequest(MoveRequest, PFQuery, PathResult);

			if (PathResult)
			{
				TArray<FNavPathPoint> PathPointsCopy;
				FNavPathSharedPtr SlicedPath;
				PathPointsCopy = PathResult->GetPathPoints();
				SlicedPath = PathResult;
				SlicedPath->GetPathPoints().Empty();

				for (int Idx = 0; Idx <= NumberOfMoves; Idx ++)
				{
					if (PathPointsCopy.IsValidIndex(Idx))
						SlicedPath->GetPathPoints().Add(PathPointsCopy[Idx]);
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Requested more moves: (%d) than path points: (%d). Idx: %d"),
						       NumberOfMoves,
						       PathPointsCopy.Num(), Idx);
						break;
					}
				}
				const FAIRequestID RequestID = PathResult.IsValid()
					                               ? RequestMove(MoveRequest, SlicedPath)
					                               : FAIRequestID::InvalidRequest;

				if (RequestID.IsValid())
				{
					bAllowStrafe = MoveRequest.CanStrafe();
					ResultData.MoveId = RequestID;
					FVector2D DestinationTile;
					GridNavigationData->WorldGridActor->GetGridCellForWorldLocation(SlicedPath->GetPathPoints().Last().Location, DestinationTile);
					GridNavigationData->WorldGridActor->UpdateActorLocationMap(
						DestinationTile, GridActorOwner);
					ResultData.Code = EPathFollowingRequestResult::RequestSuccessful;

					if (OutPath)
					{
						*OutPath = SlicedPath;
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Can't find path to: %s"), *Dest.ToString());
			}
		}
	}

	if (ResultData.Code == EPathFollowingRequestResult::Failed)
	{
		ResultData.MoveId = MyPathFollowingComponent->RequestMoveWithImmediateFinish(EPathFollowingResult::Invalid);
	}

	return ResultData;
}

// FVector2D ARoguelikeAIController::GetRoom(AActor* Actor)
// {
// 	return GridNavigationData->WorldGridActor->GetRoomCoord(GetPawn()->GetActorLocation());
// }
