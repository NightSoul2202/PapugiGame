// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Health/HealthComponent.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/HealthAttributeSet.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::InitializeHealthComponent(UAbilitySystemComponent* ASC)
{
	if (!ASC)
	{
		return;
	}

	ASC->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetHealthAttribute()).AddUObject(this, &UHealthComponent::OnHealthChanged);
}

void UHealthComponent::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.0f && Data.OldValue > 0.0f)
	{
		if (OnCharacterDeathDelegate.IsBound())
		{
			OnCharacterDeathDelegate.Broadcast();
		}
	}
}
