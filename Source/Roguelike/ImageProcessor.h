#pragma once

#include "CoreMinimal.h"
#include "ImageProcessor.generated.h"

UCLASS()
class ROGUELIKE_API UImageProcessor : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION()
	FVector GetPixelColor(FVector2D PixelCoord);

	void LoadImage(TArray<FVector>& OutImage, int&Width, int& Height, FString ImagePath);
};
