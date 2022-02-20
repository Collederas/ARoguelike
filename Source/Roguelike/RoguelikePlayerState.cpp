#include "RoguelikePlayerState.h"

void ARoguelikePlayerState::AddKeys(uint8 Amount)
{
	KeysFound = KeysFound + Amount;
}

void ARoguelikePlayerState::IncrementLevel()
{
	Level++;
}

void ARoguelikePlayerState::SetLevel(uint8 InLevel)
{
	Level = InLevel;
}

void ARoguelikePlayerState::AddKeyForCurrentLevel()
{
	KeysFoundPerLevel.Add(Level, true);
	KeysFound ++;
}

bool ARoguelikePlayerState::DidFindKeyInCurrentLevel()
{
	if (KeysFoundPerLevel.Find(Level) == nullptr)
		return false;
	return static_cast<bool>(KeysFoundPerLevel.Find(Level));
}

void ARoguelikePlayerState::ResetKeysFound()
{
	KeysFoundPerLevel.Empty();
	KeysFound = 0;
}
