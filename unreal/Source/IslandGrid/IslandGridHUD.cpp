#include "IslandGridHUD.h"

#include "AssetBeacon.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "IslandGridPawn.h"

void AIslandGridHUD::DrawCard(float X, float Y, float W,
                              const TArray<FString>& Lines,
                              const FLinearColor& Accent)
{
	const float LineH = 26.f;
	const float Pad = 16.f;
	const float H = Pad * 2 + Lines.Num() * LineH + 8.f;

	DrawRect(FLinearColor(0.03f, 0.05f, 0.08f, 0.82f), X, Y, W, H);
	DrawRect(Accent, X, Y, 4.f, H);  // accent stripe

	UFont* Font = GEngine->GetMediumFont();
	float LineY = Y + Pad;
	for (int32 i = 0; i < Lines.Num(); i++)
	{
		const bool bTitle = i == 0;
		DrawText(Lines[i],
			bTitle ? FLinearColor(Accent) : FLinearColor(0.92f, 0.96f, 0.97f),
			X + Pad + 6.f, LineY, Font, bTitle ? 1.5f : 1.1f);
		LineY += bTitle ? LineH + 8.f : LineH;
	}
}

void AIslandGridHUD::DrawHUD()
{
	Super::DrawHUD();
	if (!Canvas) return;

	const AIslandGridPawn* Pawn =
		GetOwningPlayerController()
			? Cast<AIslandGridPawn>(GetOwningPlayerController()->GetPawn())
			: nullptr;

	// control hints, bottom-left
	DrawText(TEXT("WASD fly   Q/E down/up   RMB look   wheel speed   click beacon   R restart"),
		FLinearColor(1, 1, 1, 0.55f), 24.f, Canvas->SizeY - 40.f,
		GEngine->GetMediumFont(), 1.1f);

	const AAssetBeacon* B = Pawn ? Pawn->GetFocusedBeacon() : nullptr;
	if (!B) return;

	TArray<FString> Lines;
	Lines.Add(B->Org.ToUpper());
	if (B->SiteKey == TEXT("changi"))
	{
		Lines.Add(TEXT("Energy Efficiency - flagship site"));
		Lines.Add(TEXT("Sensing: flights, passengers, weather"));
		Lines.Add(TEXT("Predicting: cooling demand ahead"));
		Lines.Add(TEXT("Acting: setpoints retuned live"));
	}
	else
	{
		Lines.Add(B->Tier == EBeaconTier::Hero
			? TEXT("Energy Management - client portfolio")
			: TEXT("Energy Management - island coverage"));
		Lines.Add(TEXT("Managed as one system, island-wide"));
		Lines.Add(TEXT("FDD: monitored - no active faults"));
	}

	const FLinearColor Accent = B->GlowColor.GetClamped(0.f, 1.f);
	DrawCard(Canvas->SizeX - 460.f, 32.f, 428.f, Lines, Accent);
}
