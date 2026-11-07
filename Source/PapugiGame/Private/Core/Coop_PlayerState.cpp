// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Coop_PlayerState.h"
#include "AbilitySystemComponent.h"
#include <GAS/Attributes/Coop_AttributeSet.h>

ACoop_PlayerState::ACoop_PlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UCoop_AttributeSet>(TEXT("AttributeSet"));

	NetUpdateFrequency = 128.0f;
}

void ACoop_PlayerState::DeathState()
{
	if (OnOutHealthDelegate.IsBound())
	{
		OnOutHealthDelegate.Broadcast();
	}
}

UAbilitySystemComponent* ACoop_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
