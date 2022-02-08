#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
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

public:	
	// Sets default values for this component's properties
	UGridMovementComponent();

	virtual void AddInputVector(FVector WorldVector, bool bForce) override;
	
	UFUNCTION(BlueprintCallable, Category="Pawn|Components|GridMovementComponent")
	virtual void Move(EMoveDirection Direction);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Config)
	int GridUnitSize = 200;
};
