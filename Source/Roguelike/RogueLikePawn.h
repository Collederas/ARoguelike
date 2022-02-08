#pragma once

#include "CoreMinimal.h"
#include "GridMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "RogueLikePawn.generated.h"

UCLASS()
class ROGUELIKE_API ARogueLikePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARogueLikePawn();

	virtual UGridMovementComponent* GetMovementComponent() const override;

	UPROPERTY(Category=Pawn, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UGridMovementComponent> MovementComponent;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void MoveForward();
	void MoveBackward();
	void MoveLeft();
	void MoveRight();
};
