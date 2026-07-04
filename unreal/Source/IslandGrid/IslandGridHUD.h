#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "IslandGridHUD.generated.h"

/** Canvas-drawn asset card for the focused beacon + control hints. */
UCLASS()
class ISLANDGRID_API AIslandGridHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	void DrawCard(float X, float Y, float W, const TArray<FString>& Lines,
	              const FLinearColor& Accent);
};
