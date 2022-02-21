#include "RoguelikeGameInstance.h"

void URoguelikeGameInstance::AddKeys(uint8 Amount)
{
	KeysFound = KeysFound + Amount;
}

void URoguelikeGameInstance::IncrementLevel()
{
	Level++;
}

void URoguelikeGameInstance::SetLevel(int InLevel)
{
	Level = InLevel;
}

void URoguelikeGameInstance::AddKeyForCurrentLevel()
{
	KeysFoundPerLevel.Add(Level, true);
	KeysFound ++;
}

bool URoguelikeGameInstance::DidFindKeyInCurrentLevel()
{
	if (KeysFoundPerLevel.Find(Level) == nullptr)
		return false;
	return static_cast<bool>(KeysFoundPerLevel.Find(Level));
}

void URoguelikeGameInstance::ResetKeysFound()
{
	KeysFoundPerLevel.Empty();
	KeysFound = 0;
}
