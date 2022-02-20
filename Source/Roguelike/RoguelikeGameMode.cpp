#include "RoguelikeGameMode.h"

#include "LevelSpawner.h"
#include "WorldGrid.h"
#include "Kismet/GameplayStatics.h"
#include "Roguelike/Characters/RoguelikeCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARoguelikeGameMode::ARoguelikeGameMode()
{
	// set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Chars/BP_Player"));
	// if (PlayerPawnBPClass.Class != NULL)
	// {
	// 	DefaultPawnClass = PlayerPawnBPClass.Class;
	// }
}


