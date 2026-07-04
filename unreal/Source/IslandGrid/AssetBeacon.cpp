#include "AssetBeacon.h"

#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

AAssetBeacon::AAssetBeacon()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Pillar = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pillar"));
	Pillar->SetupAttachment(Root);
	// clickable via the visibility channel only — never blocks flight or the altitude guard
	Pillar->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Pillar->SetCollisionResponseToAllChannels(ECR_Ignore);
	Pillar->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Pillar->SetMobility(EComponentMobility::Movable);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		Pillar->SetStaticMesh(CylinderMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> GlowMat(
		TEXT("/Game/Materials/M_Beacon.M_Beacon"));
	if (GlowMat.Succeeded())
	{
		Pillar->SetMaterial(0, GlowMat.Object);
	}

	Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Label"));
	Label->SetupAttachment(Root);
	Label->SetHorizontalAlignment(EHTA_Center);
	Label->SetTextRenderColor(FColor(234, 246, 248));
	Label->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAssetBeacon::ApplyAppearance()
{
	const bool bHero = Tier == EBeaconTier::Hero;
	const float HeightM = bHero ? 380.f : 220.f;  // rises above the CBD skyline
	const float WidthM = bHero ? 16.f : 9.f;

	// engine cylinder is 1m x 1m; scale is in metres directly
	Pillar->SetRelativeScale3D(FVector(WidthM, WidthM, HeightM));
	Pillar->SetRelativeLocation(FVector(0, 0, HeightM * 50.f));

	Label->SetText(FText::FromString(Org));
	Label->SetWorldSize(bHero ? 5200.f : 3200.f);
	Label->SetRelativeLocation(FVector(0, 0, HeightM * 100.f + 4500.f));
}

void AAssetBeacon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyAppearance();
}

void AAssetBeacon::BeginPlay()
{
	Super::BeginPlay();
	ApplyAppearance();
	GlowMID = Pillar->CreateAndSetMaterialInstanceDynamic(0);
	PulsePhase = FMath::FRandRange(0.f, 6.28f);
}

void AAssetBeacon::SetSelected(bool bSelected)
{
	bIsSelected = bSelected;
}

void AAssetBeacon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PulsePhase += DeltaSeconds * (bIsSelected ? 6.f : 2.2f);
	const float Pulse = 0.65f + 0.35f * FMath::Sin(PulsePhase);
	const float Boost = bIsSelected ? 2.4f : 1.f;
	if (GlowMID)
	{
		GlowMID->SetVectorParameterValue(TEXT("Emissive"), GlowColor * Pulse * Boost);
	}

	// billboard the label toward the player camera
	if (const APlayerCameraManager* Cam =
			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0))
	{
		const FVector ToCam = Cam->GetCameraLocation() - Label->GetComponentLocation();
		Label->SetWorldRotation(ToCam.Rotation());
	}
}
