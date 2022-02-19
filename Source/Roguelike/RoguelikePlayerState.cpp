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
