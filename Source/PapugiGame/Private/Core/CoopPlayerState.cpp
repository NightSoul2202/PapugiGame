// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/CoopPlayerState.h"
#include "AbilitySystemComponent.h"
#include <GAS/Attributes/CoopAttributeSet.h>

ACoopPlayerState::ACoopPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UCoopAttributeSet>(TEXT("AttributeSet"));

	NetUpdateFrequency = 128.0f;
}

void ACoopPlayerState::DeathState()
{
	if (OnOutHealthDelegate.IsBound())
	{
		OnOutHealthDelegate.Broadcast();
	}
}

UAbilitySystemComponent* ACoopPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
