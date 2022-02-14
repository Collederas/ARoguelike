#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "GridMovementComponent.generated.h"

UENUM(BlueprintType)
enum class EMoveDirection : uint8 {
	Forward,
	Backward,
	Left,
	Right
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Config=Game)
class ROGUELIKE_API UGridMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	DECLARE_MULTICAST_DELEGATE_OneParam(FMoveComplete, const FPathFollowingResult& /*Result*/);

public:
	// Sets default values for this component's properties
	UGridMovementComponent();

	FMoveComplete OnMoveComplete;

	UFUNCTION(BlueprintCallable, Category="Pawn|Components|GridMovementComponent")
	FVector RequestMove(EMoveDirection Direction);

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
	float MoveDuration = 1.f;
	
	UPROPERTY(BlueprintReadOnly)
	FVector RequestedMove;
	
	UPROPERTY(Config)
	int GridUnitSize = 200;

private:
	float MoveTimeElapsed = 0;
};
