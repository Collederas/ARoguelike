#pragma once

#include "CoreMinimal.h"
#include "Roguelike/Navigation/GridMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "RogueLikePawn.generated.h"

UCLASS()
class ROGUELIKE_API ARogueLikePawn : public APawn
{
	GENERATED_BODY()

public:
	ARogueLikePawn();

	virtual UGridMovementComponent* GetMovementComponent() const override;

	UPROPERTY(Category=Pawn, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UGridMovementComponent> MovementComponent;

protected:
	UFUNCTION(BlueprintCallable)
	FVector MoveForward();

	UFUNCTION(BlueprintCallable)
	FVector MoveBackward();

	UFUNCTION(BlueprintCallable)
	FVector MoveLeft();

	UFUNCTION(BlueprintCallable)
	FVector MoveRight();
};
