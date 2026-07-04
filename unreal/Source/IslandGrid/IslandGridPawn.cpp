#include "IslandGridPawn.h"

#include "AssetBeacon.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"

AIslandGridPawn::AIslandGridPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Root);

	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
	Movement->MaxSpeed = 120000.f;      // city-scale flight
	Movement->Acceleration = 240000.f;
	Movement->Deceleration = 360000.f;
}

void AIslandGridPawn::BeginPlay()
{
	Super::BeginPlay();

	// look down over the island on spawn
	AddActorLocalRotation(FRotator(0, 0, 0));
	if (AController* C = GetController())
	{
		C->SetControlRotation(FRotator(-38.f, 0.f, 0.f));
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		FInputModeGameAndUI Mode;
		Mode.SetHideCursorDuringCapture(true);
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(Mode);
	}
}

void AIslandGridPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	Ctx = NewObject<UInputMappingContext>(this);
	IA_Move = NewObject<UInputAction>(this);
	IA_Move->ValueType = EInputActionValueType::Axis2D;
	IA_Elevate = NewObject<UInputAction>(this);
	IA_Elevate->ValueType = EInputActionValueType::Axis1D;
	IA_Look = NewObject<UInputAction>(this);
	IA_Look->ValueType = EInputActionValueType::Axis2D;
	IA_LookBtn = NewObject<UInputAction>(this);
	IA_LookBtn->ValueType = EInputActionValueType::Boolean;
	IA_Click = NewObject<UInputAction>(this);
	IA_Click->ValueType = EInputActionValueType::Boolean;
	IA_Speed = NewObject<UInputAction>(this);
	IA_Speed->ValueType = EInputActionValueType::Axis1D;

	auto MapKey = [this](UInputAction* Action, FKey Key, bool bNegate = false,
	                     bool bSwizzle = false)
	{
		FEnhancedActionKeyMapping& M = Ctx->MapKey(Action, Key);
		if (bNegate)
		{
			UInputModifierNegate* Neg = NewObject<UInputModifierNegate>(Ctx);
			M.Modifiers.Add(Neg);
		}
		if (bSwizzle)
		{
			UInputModifierSwizzleAxis* Sw = NewObject<UInputModifierSwizzleAxis>(Ctx);
			Sw->Order = EInputAxisSwizzle::YXZ;
			M.Modifiers.Add(Sw);
		}
	};

	MapKey(IA_Move, EKeys::W, false, true);   // forward -> Y
	MapKey(IA_Move, EKeys::S, true, true);
	MapKey(IA_Move, EKeys::D);
	MapKey(IA_Move, EKeys::A, true);
	MapKey(IA_Elevate, EKeys::E);
	MapKey(IA_Elevate, EKeys::Q, true);
	MapKey(IA_Look, EKeys::Mouse2D);
	MapKey(IA_LookBtn, EKeys::RightMouseButton);
	MapKey(IA_Click, EKeys::LeftMouseButton);
	MapKey(IA_Speed, EKeys::MouseWheelAxis);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (auto* Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
				PC->GetLocalPlayer()))
		{
			Sub->AddMappingContext(Ctx, 0);
		}
	}

	auto* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AIslandGridPawn::OnMove);
	EIC->BindAction(IA_Elevate, ETriggerEvent::Triggered, this, &AIslandGridPawn::OnElevate);
	EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AIslandGridPawn::OnLook);
	EIC->BindAction(IA_LookBtn, ETriggerEvent::Started, this, &AIslandGridPawn::OnLookStart);
	EIC->BindAction(IA_LookBtn, ETriggerEvent::Completed, this, &AIslandGridPawn::OnLookStop);
	EIC->BindAction(IA_Click, ETriggerEvent::Started, this, &AIslandGridPawn::OnClick);
	EIC->BindAction(IA_Speed, ETriggerEvent::Triggered, this, &AIslandGridPawn::OnSpeed);
}

void AIslandGridPawn::OnMove(const FInputActionValue& Value)
{
	const FVector2D V = Value.Get<FVector2D>();
	const FRotator Yaw(0, GetControlRotation().Yaw, 0);
	AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), V.Y);
	AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y), V.X);
	bFlying = false;
}

void AIslandGridPawn::OnElevate(const FInputActionValue& Value)
{
	AddMovementInput(FVector::UpVector, Value.Get<float>());
	bFlying = false;
}

void AIslandGridPawn::OnLookStart(const FInputActionValue&) { bLooking = true; }
void AIslandGridPawn::OnLookStop(const FInputActionValue&) { bLooking = false; }

void AIslandGridPawn::OnLook(const FInputActionValue& Value)
{
	if (!bLooking) return;
	const FVector2D V = Value.Get<FVector2D>();
	AddControllerYawInput(V.X * 0.6f);
	AddControllerPitchInput(-V.Y * 0.6f);
}

void AIslandGridPawn::OnSpeed(const FInputActionValue& Value)
{
	const float F = FMath::Clamp(
		Movement->MaxSpeed * (1.f + 0.18f * Value.Get<float>()), 3000.f, 600000.f);
	Movement->MaxSpeed = F;
	Movement->Acceleration = F * 2.f;
}

void AIslandGridPawn::OnClick(const FInputActionValue&)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;
	FHitResult Hit;
	if (!PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit)) return;
	AAssetBeacon* Beacon = Cast<AAssetBeacon>(Hit.GetActor());
	if (!Beacon) return;

	if (Focused) Focused->SetSelected(false);
	Focused = Beacon;
	Focused->SetSelected(true);

	// park the camera at a cinematic offset from the beacon
	const FVector Target = Beacon->GetActorLocation();
	const FVector Dir = (GetActorLocation() - Target).GetSafeNormal2D();
	FlyTarget = Target + Dir * 65000.f + FVector(0, 0, 32000.f);
	bFlying = true;
}

void AIslandGridPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bFlying)
	{
		const FVector P = FMath::VInterpTo(GetActorLocation(), FlyTarget, DeltaSeconds, 1.6f);
		SetActorLocation(P);
		if (Focused)
		{
			const FRotator Want =
				(Focused->GetActorLocation() - P + FVector(0, 0, 12000.f)).Rotation();
			if (AController* C = GetController())
			{
				C->SetControlRotation(
					FMath::RInterpTo(C->GetControlRotation(), Want, DeltaSeconds, 2.2f));
			}
		}
		if (FVector::Dist(P, FlyTarget) < 500.f) bFlying = false;
	}
}
