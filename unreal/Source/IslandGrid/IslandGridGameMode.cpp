#include "IslandGridGameMode.h"

#include "IslandGridHUD.h"
#include "IslandGridPawn.h"

AIslandGridGameMode::AIslandGridGameMode()
{
	DefaultPawnClass = AIslandGridPawn::StaticClass();
	HUDClass = AIslandGridHUD::StaticClass();
}
