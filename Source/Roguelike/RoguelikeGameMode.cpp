#include "RoguelikeGameMode.h"
#include "Roguelike/Characters/RoguelikeCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARoguelikeGameMode::ARoguelikeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
