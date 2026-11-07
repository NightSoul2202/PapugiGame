// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Interaction/InteractionComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "Interfaces/Interactable.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	APawn* OwnerPawn = Cast<APawn>(GetOwner());

	if (OwnerPawn && OwnerPawn->IsLocallyControlled())
	{
		if (UWorld* World = GetWorld())
		{
			float ActualFrequency = (TraceFrequency > 0.0f) ? TraceFrequency : 0.1f;

			World->GetTimerManager().SetTimer(
				TimerHandle_Interaction,
				this,
				&UInteractionComponent::UpdateFocus,
				ActualFrequency,
				true
			);
		}
	}
}

void UInteractionComponent::UpdateFocus()
{
	AActor* HitInteractable = PerformInteractionTrace();

	ProcessNewFocus(HitInteractable);
}

bool UInteractionComponent::GetTracePoints(FVector& OutStart, FVector& OutEnd) const
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (!PawnOwner || !PawnOwner->GetController())
	{
		return false;
	}

	FRotator ViewRotation;
	PawnOwner->GetController()->GetPlayerViewPoint(OutStart, ViewRotation);
	OutEnd = OutStart + (ViewRotation.Vector() * InteractionDistance);

	return true;
}

TObjectPtr<AActor> UInteractionComponent::PerformInteractionTrace() const
{
	FVector TraceStart, TraceEnd;
	if (!GetTracePoints(TraceStart, TraceEnd))
	{
		return nullptr;
	}

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());

	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, CollisionParams);

	if (bHit && HitResult.GetActor() && HitResult.GetActor()->Implements<UInteractable>())
	{
		return HitResult.GetActor();
	}

	return nullptr;
}

void UInteractionComponent::ProcessNewFocus(AActor* NewFocus)
{
	if (NewFocus != CurrentFocus)
	{
		CurrentFocus = NewFocus;
		OnFocusChanged.Broadcast(CurrentFocus);
	}
}

void UInteractionComponent::TryInteract()
{
	if (CurrentFocus)
	{
		if (GetOwner()->HasAuthority())
		{ 
			IInteractable::Execute_Interact(CurrentFocus, GetOwner());
		}
		else
		{
			Server_Interact(CurrentFocus);
		}
		
	}
}

void UInteractionComponent::Server_Interact_Implementation(AActor* InteractableTarget)
{
	if (InteractableTarget)
	{
		IInteractable::Execute_Interact(InteractableTarget, GetOwner());
	}
}

