// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Coop_PlayerState.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/HealthAttributeSet.h"
#include "GAS/Attributes/MovementAttributeSet.h"
#include "Net/UnrealNetwork.h"

ACoop_PlayerState::ACoop_PlayerState()
{
	bReplicates = true;

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

void ACoop_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACoop_PlayerState, SelectedClassData);
}

UHeroClassInfo* ACoop_PlayerState::GetHeroClassData() const
{
	return SelectedClassData;
}

void ACoop_PlayerState::OnRep_SelectedClass()
{
	if (OnPlayerClassUpdatedDelegate.IsBound())
	{
		OnPlayerClassUpdatedDelegate.Broadcast(SelectedClassData);
	}
}
