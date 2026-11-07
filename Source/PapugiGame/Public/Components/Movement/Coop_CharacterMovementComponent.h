// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Coop_CharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum class EMovementAction : uint8
{
	None,
	Sprint,
	Slide,
};

DECLARE_MULTICAST_DELEGATE(FOnSlideExitedSignature);

UCLASS()
class PAPUGIGAME_API UCoop_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

private:
	float SprintTime = 0.0f;
	float CurrentSlideTime = 0.0f;

	void UpdateSlideFloor();
	bool IsReachedSlideLimits(float CurrentSpeed);
	void ExitSlide();
	float CalculateFrictionForSlide();
	float ApplySlideInputAcceleration(float deltaTime, float CurrentSpeed, float CurrentFriction);
	void ApplySlideSlopeBoost(float deltaTime);
	void PerformSlideMovement(float deltaTime);

public:
	FOnSlideExitedSignature OnSlideExitedDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
	float MaxSprintSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
	float MinSprintSpeed = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
	float MinSlideSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
	float SlideBoost = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
	float SlideFriction = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
	float MinSlideDuration = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
	float MaxSlidingFrictionMultiplier = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
	float MinSlidingFrictionMultiplier = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
	float SteeringForce = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
	float UphillResistance = 30.0f;

	EMovementAction MovementAction;

	void SetCustomMovementAction(EMovementAction MovementActionToTrigger);
	void PhysSlide(float deltaTime, int32 Iterations);

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelociry);
	virtual float GetMaxSpeed() const override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	class FSaveMove_Coop : public FSavedMove_Character
	{
	public:
		typedef FSavedMove_Character Super;

		EMovementAction SavedMovementAction = EMovementAction::None;

		virtual void Clear() override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
		virtual bool CanCombineWith(const FSavedMovePtr& NewCoopMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual uint8 GetCompressedFlags() const override;
	};

	class FNetworkPredictionData_Client_Coop : public FNetworkPredictionData_Client_Character
	{
	public:
		typedef FNetworkPredictionData_Client_Character Super;

		FNetworkPredictionData_Client_Coop(const UCharacterMovementComponent& ClientMovement);

		virtual FSavedMovePtr AllocateNewMove() override;
	};
};
