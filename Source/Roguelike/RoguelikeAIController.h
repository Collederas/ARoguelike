// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RoguelikeCharacter.h"
#include "WorldGrid.h"
#include "Navigation/GridNavigationData.h"
#include "RoguelikeAIController.generated.h"

/**
 * 
 */
UCLASS()
class ROGUELIKE_API ARoguelikeAIController : public AAIController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	virtual void RandomReachablePointInCurrentRoom(FVector& RandomLocation);

	// Gets room coordinates where actor is
	UFUNCTION(BlueprintCallable)
	virtual FVector2D GetRoom(AActor* Actor);

protected:
	UPROPERTY(BlueprintReadOnly)
	AGridNavigationData* GridNavigationData;
};

