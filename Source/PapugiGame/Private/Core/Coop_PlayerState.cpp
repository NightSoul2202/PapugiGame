// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Coop_PlayerState.h"
#include "AbilitySystemComponent.h"
#include <GAS/Attributes/HealthAttributeSet.h>
#include <GAS/Attributes/MovementAttributeSet.h>

ACoop_PlayerState::ACoop_PlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	HealthAttributeSet = CreateDefaultSubobject<UHealthAttributeSet>(TEXT("HealthAttributeSet"));
	MovementAttributeSet = CreateDefaultSubobject<UMovementAttributeSet>(TEXT("MovementAttributeSet"));

	NetUpdateFrequency = 128.0f;
}

UAbilitySystemComponent* ACoop_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
