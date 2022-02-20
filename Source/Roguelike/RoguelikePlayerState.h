#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RoguelikePlayerState.generated.h"


UCLASS()
class ROGUELIKE_API ARoguelikePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	int KeysFound;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	int Level = 0;

	UFUNCTION(BlueprintCallable)
	void AddKeys(uint8 Amount);

	UFUNCTION(BlueprintCallable)
	void IncrementLevel();
	
	UFUNCTION(BlueprintCallable)
	void SetLevel(uint8 InLevel);
	
	UFUNCTION(BlueprintCallable)
	void AddKeyForCurrentLevel();
	
	UFUNCTION(BlueprintCallable)
	bool DidFindKeyInCurrentLevel();
	
	UFUNCTION(BlueprintCallable)
	void ResetKeysFound();
	
private:
	UPROPERTY()
	TMap<int, bool> KeysFoundPerLevel;
};
