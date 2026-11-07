// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Movement/Coop_CharacterMovementComponent.h"
#include "Characters/Coop_PlayerCharacter.h"
#include "GameFramework/Character.h"

void UCoop_CharacterMovementComponent::UpdateSlideFloor()
{
	FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, false, nullptr);
	if (!CurrentFloor.IsWalkableFloor())
	{
		ExitSlide();
		return;
	}
}

bool UCoop_CharacterMovementComponent::IsReachedSlideLimits(float CurrentSpeed)
{
	if ((CurrentSlideTime > MinSlideDuration) || CurrentSpeed < MinSlideSpeed)
	{
		ExitSlide();
		return false;
	}
	return true;
}

void UCoop_CharacterMovementComponent::ExitSlide()
{
	MovementAction = EMovementAction::None;

	SetMovementMode(CurrentFloor.IsWalkableFloor() ? MOVE_Walking : MOVE_Falling);

	if (OnSlideExitedDelegate.IsBound())
	{
		OnSlideExitedDelegate.Broadcast();
	}
}

float UCoop_CharacterMovementComponent::CalculateFrictionForSlide()
{
	return (CurrentSlideTime < MinSlideDuration) ? SlideFriction * MinSlidingFrictionMultiplier : SlideFriction * MaxSlidingFrictionMultiplier;
}

float UCoop_CharacterMovementComponent::ApplySlideInputAcceleration(float deltaTime, float CurrentSpeed, float CurrentFriction)
{
	FVector InputDir = Acceleration.GetSafeNormal2D();

	if (InputDir.IsNearlyZero())
	{
		return CurrentFriction;
	}

	float Dot = FVector::DotProduct(InputDir, Velocity.GetSafeNormal2D());
	if (Dot < -0.1f)
	{
		return SlideFriction * MaxSlidingFrictionMultiplier;
	}

	Velocity += InputDir * SteeringForce * deltaTime;
	Velocity = Velocity.GetSafeNormal2D() * CurrentSpeed;

	return CurrentFriction;
}

void UCoop_CharacterMovementComponent::ApplySlideSlopeBoost(float deltaTime)
{
	FVector FloorNormal = CurrentFloor.HitResult.Normal;
	FVector GravityDir = FVector(0.f, 0.f, -1.f);
	FVector SlopeRight = FVector::CrossProduct(GravityDir, FloorNormal).GetSafeNormal();
	FVector DownhillDirection = FVector::CrossProduct(FloorNormal, SlopeRight).GetSafeNormal();
	float SlopeSteepness = 1.0f - FloorNormal.Z;

	float MovementDot = FVector::DotProduct(Velocity.GetSafeNormal(), DownhillDirection);

	FVector SlopeForce = DownhillDirection * SlideBoost * SlopeSteepness * deltaTime;

	Velocity += (MovementDot < 0.0f) ? SlopeForce * UphillResistance : SlopeForce;
}

void UCoop_CharacterMovementComponent::PerformSlideMovement(float deltaTime)
{
	FVector ProjectedVelocity = FVector::VectorPlaneProject(Velocity, CurrentFloor.HitResult.Normal);
	FVector AdjustedVelocity = ProjectedVelocity * deltaTime;

	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(AdjustedVelocity, UpdatedComponent->GetComponentQuat(), true, Hit);

	if (Hit.IsValidBlockingHit())
	{
		HandleImpact(Hit, deltaTime, AdjustedVelocity);
		SlideAlongSurface(AdjustedVelocity, 1.f - Hit.Time, Hit.Normal, Hit, true);
	}
}

void UCoop_CharacterMovementComponent::SetCustomMovementAction(EMovementAction MovementActionToTrigger)
{
	if (MovementAction == EMovementAction::Slide && MovementActionToTrigger != EMovementAction::Slide)
	{
		return;
	}

	MovementAction = MovementActionToTrigger;

	switch (MovementActionToTrigger)
	{
	case EMovementAction::Slide:
		SetMovementMode(MOVE_Custom);
		CurrentSlideTime = 0.0f;
		Velocity += Velocity.GetSafeNormal2D() * 600.f;
		break;
	case EMovementAction::Sprint:
		break;
	default:
		if (MovementMode == MOVE_Custom)
		{
			SetMovementMode(MOVE_Walking);
		}
		break;
	}
}

void UCoop_CharacterMovementComponent::PhysSlide(float deltaTime, int32 Iterations)
{
	if (Iterations >= MaxSimulationIterations || deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	CurrentSlideTime += deltaTime;

	UpdateSlideFloor();

	float CurrentSpeed = Velocity.Size2D();

	if (!IsReachedSlideLimits(CurrentSpeed))
	{
		return;
	}

	float DynamicFriction = CalculateFrictionForSlide();
	DynamicFriction = ApplySlideInputAcceleration(deltaTime, CurrentSpeed, DynamicFriction);

	Velocity = Velocity.GetSafeNormal2D() * FMath::Max(CurrentSpeed - (DynamicFriction * deltaTime), 0.f);

	ApplySlideSlopeBoost(deltaTime);

	PerformSlideMovement(deltaTime);
}

void UCoop_CharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	MovementAction = static_cast<EMovementAction>((Flags >> 4) & 0x0F);

	if (MovementAction != EMovementAction::Slide && MovementMode == MOVE_Custom)
	{
		SetMovementMode(CurrentFloor.IsWalkableFloor() ? MOVE_Walking : MOVE_Falling);
	}
}

FNetworkPredictionData_Client* UCoop_CharacterMovementComponent::GetPredictionData_Client() const
{
	if (!ClientPredictionData)
	{
		UCoop_CharacterMovementComponent* MutableThis = const_cast<UCoop_CharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Coop(*this);
	}

	return ClientPredictionData;
}

void UCoop_CharacterMovementComponent::FSaveMove_Coop::Clear()
{
	Super::Clear();

	SavedMovementAction = EMovementAction::None;
}

void UCoop_CharacterMovementComponent::FSaveMove_Coop::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	if (UCoop_CharacterMovementComponent* CharacterMovement = Cast<UCoop_CharacterMovementComponent>(C->GetCharacterMovement()))
	{
		SavedMovementAction = CharacterMovement->MovementAction;
	}
}

void UCoop_CharacterMovementComponent::FSaveMove_Coop::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	if (UCoop_CharacterMovementComponent* CharacterMovement = Cast<UCoop_CharacterMovementComponent>(C->GetCharacterMovement()))
	{
		CharacterMovement->MovementAction = SavedMovementAction;
	}
}

bool UCoop_CharacterMovementComponent::FSaveMove_Coop::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	FSaveMove_Coop* NewCoopMove = static_cast<FSaveMove_Coop*>(NewMove.Get());

	if (SavedMovementAction != NewCoopMove->SavedMovementAction)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

uint8 UCoop_CharacterMovementComponent::FSaveMove_Coop::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	Result |= (static_cast<uint8>(SavedMovementAction) & 0x0F) << 4;

	return Result;
}

void UCoop_CharacterMovementComponent::OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaTime, OldLocation, OldVelocity);

	if (!CharacterOwner)
	{
		return;
	}

	float TransitionSpeed = 2.0f;

	SprintTime = (MovementAction == EMovementAction::Sprint) 
		? FMath::Min(SprintTime + (DeltaTime * TransitionSpeed), 1.0f) 
		: FMath::Max(SprintTime - (DeltaTime * TransitionSpeed), 0.0f);
}

float UCoop_CharacterMovementComponent::GetMaxSpeed() const
{
	if (SprintTime > 0.0f)
	{
		return FMath::Lerp(MaxWalkSpeed, MaxSprintSpeed, SprintTime);
	}
	
	return Super::GetMaxSpeed();
}

void UCoop_CharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	if (!CharacterOwner)
	{
		return;
	}

	switch (MovementAction)
	{
	case EMovementAction::Slide:
		PhysSlide(deltaTime, Iterations);
		break;
	default: 
		break;
	}

	Super::PhysCustom(deltaTime, Iterations);
}

FSavedMovePtr UCoop_CharacterMovementComponent::FNetworkPredictionData_Client_Coop::AllocateNewMove()
{
	return FSavedMovePtr(new FSaveMove_Coop());
}

UCoop_CharacterMovementComponent::FNetworkPredictionData_Client_Coop::FNetworkPredictionData_Client_Coop(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}
	