#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RoguelikeGameInstance.generated.h"


UCLASS()
class ROGUELIKE_API URoguelikeGameInstance : public UGameInstance
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
	void SetLevel(int InLevel);
	
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
