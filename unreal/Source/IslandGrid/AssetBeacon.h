#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AssetBeacon.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;
class UMaterialInstanceDynamic;

UENUM(BlueprintType)
enum class EBeaconTier : uint8
{
	Hero,
	Mock
};

/** Glowing, clickable asset marker. Pillar pulses; label always faces the camera. */
UCLASS()
class ISLANDGRID_API AAssetBeacon : public AActor
{
	GENERATED_BODY()

public:
	AAssetBeacon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset")
	FString SiteKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset")
	FString Org;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset")
	EBeaconTier Tier = EBeaconTier::Mock;

	/** Amber for Changi, cyan otherwise. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset")
	FLinearColor GlowColor = FLinearColor(0.10f, 3.2f, 2.9f);

	void SetSelected(bool bSelected);
	void ApplyAppearance();

	virtual void Tick(float DeltaSeconds) override;
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Pillar;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTextRenderComponent> Label;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> GlowMID;

	bool bIsSelected = false;
	float PulsePhase = 0.f;
};
