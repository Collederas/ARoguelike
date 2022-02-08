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

void ARogueLikePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARogueLikePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARogueLikePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("MoveForward", IE_Pressed, this, &ARogueLikePawn::MoveForward);
	PlayerInputComponent->BindAction("MoveBackward", IE_Pressed, this, &ARogueLikePawn::MoveBackward);
	PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ARogueLikePawn::MoveLeft);
	PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ARogueLikePawn::MoveRight);
}

void ARogueLikePawn::MoveForward()
{
	MovementComponent->Move(EMoveDirection::Forward);
}

void ARogueLikePawn::MoveBackward()
{
	MovementComponent->Move(EMoveDirection::Backward);
}

void ARogueLikePawn::MoveLeft()
{
	MovementComponent->Move(EMoveDirection::Left);
}

void ARogueLikePawn::MoveRight()
{
	MovementComponent->Move(EMoveDirection::Right);
}