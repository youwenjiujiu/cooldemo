#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "IslandGridPawn.generated.h"

class UCameraComponent;
class UFloatingPawnMovement;
class UInputAction;
class UInputMappingContext;
class AAssetBeacon;
struct FInputActionValue;

/**
 * Free-fly explorer pawn: WASD + QE elevate, hold RMB to look, wheel = speed,
 * LMB click on a beacon = smooth fly-to focus.
 */
UCLASS()
class ISLANDGRID_API AIslandGridPawn : public APawn
{
	GENERATED_BODY()

public:
	AIslandGridPawn();

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	AAssetBeacon* GetFocusedBeacon() const { return Focused; }

protected:
	virtual void BeginPlay() override;

	void OnMove(const FInputActionValue& Value);
	void OnElevate(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnLookStart(const FInputActionValue& Value);
	void OnLookStop(const FInputActionValue& Value);
	void OnClick(const FInputActionValue& Value);
	void OnSpeed(const FInputActionValue& Value);
	void OnReset(const FInputActionValue& Value);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UFloatingPawnMovement> Movement;

	// runtime-built enhanced input (no asset dependencies)
	UPROPERTY() TObjectPtr<UInputMappingContext> Ctx;
	UPROPERTY() TObjectPtr<UInputAction> IA_Move;
	UPROPERTY() TObjectPtr<UInputAction> IA_Elevate;
	UPROPERTY() TObjectPtr<UInputAction> IA_Look;
	UPROPERTY() TObjectPtr<UInputAction> IA_LookBtn;
	UPROPERTY() TObjectPtr<UInputAction> IA_Click;
	UPROPERTY() TObjectPtr<UInputAction> IA_Speed;
	UPROPERTY() TObjectPtr<UInputAction> IA_Reset;

	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;

	UPROPERTY()
	TObjectPtr<AAssetBeacon> Focused;

	bool bLooking = false;
	FVector FlyTarget = FVector::ZeroVector;
	bool bFlying = false;
};
