// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Attributes/MovementAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UMovementAttributeSet::UMovementAttributeSet()
{
	InitSpeedSprint(1200.0f);
	InitMaxSpeedSprint(1200.0f);
}

void UMovementAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UMovementAttributeSet, SpeedSprint, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMovementAttributeSet, MaxSpeedSprint, COND_None, REPNOTIFY_Always);
}

void UMovementAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetSpeedSprintAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 600.0f, GetMaxSpeedSprint());
	}
}

void UMovementAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetSpeedSprintAttribute())
	{
		SetSpeedSprint(FMath::Clamp(GetSpeedSprint(), 0.0f, GetMaxSpeedSprint()));
	}
}

void UMovementAttributeSet::OnRep_SpeedSprint(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMovementAttributeSet, SpeedSprint, OldSpeed);
}

void UMovementAttributeSet::OnRep_MaxSpeedSprint(const FGameplayAttributeData& MaxOldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMovementAttributeSet, MaxSpeedSprint, MaxOldSpeed);
}
