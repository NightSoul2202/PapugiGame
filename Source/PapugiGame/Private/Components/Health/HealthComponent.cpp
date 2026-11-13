// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Health/HealthComponent.h"
#include "Core/Coop_PlayerState.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::InitializeHealthComponent(ACoop_PlayerState* PS)
{
	if (PS)
	{
		PS->OnOutHealthDelegate.AddUObject(this, &UHealthComponent::OnPlayerStateDeath);
	}
}

void UHealthComponent::OnPlayerStateDeath()
{
	if (OnCharacterDeathDelegate.IsBound())
	{
		OnCharacterDeathDelegate.Broadcast();
	}
}
