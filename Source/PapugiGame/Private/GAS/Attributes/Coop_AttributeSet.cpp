// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/Coop_AttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "Core/Coop_PlayerState.h"

UCoop_AttributeSet::UCoop_AttributeSet()
{
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitSpeedSprint(1200.0f);
	InitMaxSpeedSprint(1200.0f);
	InitArmor(100.0f);
}

void UCoop_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCoop_AttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoop_AttributeSet, SpeedSprint, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoop_AttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoop_AttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoop_AttributeSet, MaxSpeedSprint, COND_None, REPNOTIFY_Always);
}

void UCoop_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetSpeedSprintAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 600.0f, GetMaxSpeedSprint());
	}
}

void UCoop_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetSpeedSprintAttribute())
	{
		SetSpeedSprint(FMath::Clamp(GetSpeedSprint(), 0.0f, GetMaxSpeedSprint()));
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		PerformIncomingDamage();
	}
}

void UCoop_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoop_AttributeSet, Health, OldHealth);

	HandleDeath();
}

void UCoop_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoop_AttributeSet, MaxHealth, OldMaxHealth);
}

void UCoop_AttributeSet::OnRep_SpeedSprint(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoop_AttributeSet, SpeedSprint, OldSpeed);
}

void UCoop_AttributeSet::OnRep_MaxSpeedSprint(const FGameplayAttributeData& MaxOldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoop_AttributeSet, MaxSpeedSprint, MaxOldSpeed);
}

void UCoop_AttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoop_AttributeSet, Armor, OldArmor);
}

void UCoop_AttributeSet::PerformIncomingDamage()
{
	const float LocalIncomingDamage = GetIncomingDamage();

	SetIncomingDamage(0.0f);

	if (LocalIncomingDamage > 0.0f)
	{
		const float NewHealth = GetHealth() - LocalIncomingDamage;

		SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));

		HandleDeath();
	}
}

void UCoop_AttributeSet::HandleDeath()
{
	if (GetHealth() <= 0.0f)
	{
		if (ACoop_PlayerState* PS = Cast<ACoop_PlayerState>(GetOwningActor()))
		{
			PS->DeathState();
		}
	}
}
