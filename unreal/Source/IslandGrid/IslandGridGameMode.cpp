#include "IslandGridGameMode.h"

#include "IslandGridPawn.h"

AIslandGridGameMode::AIslandGridGameMode()
{
	DefaultPawnClass = AIslandGridPawn::StaticClass();
}
