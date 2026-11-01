// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributes/CoopAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "Core/CoopPlayerState.h"

UCoopAttributeSet::UCoopAttributeSet()
{
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	InitSpeedSprint(1200.0f);
	InitMaxSpeedSprint(1200.0f);
	InitArmor(100.0f);
}

void UCoopAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UCoopAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoopAttributeSet, SpeedSprint, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoopAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoopAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCoopAttributeSet, MaxSpeedSprint, COND_None, REPNOTIFY_Always);
}

void UCoopAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
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

void UCoopAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
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

void UCoopAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoopAttributeSet, Health, OldHealth);

	HandleDeath();
}

void UCoopAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoopAttributeSet, MaxHealth, OldMaxHealth);
}

void UCoopAttributeSet::OnRep_SpeedSprint(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoopAttributeSet, SpeedSprint, OldSpeed);
}

void UCoopAttributeSet::OnRep_MaxSpeedSprint(const FGameplayAttributeData& MaxOldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoopAttributeSet, MaxSpeedSprint, MaxOldSpeed);
}

void UCoopAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCoopAttributeSet, Armor, OldArmor);
}

void UCoopAttributeSet::PerformIncomingDamage()
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

void UCoopAttributeSet::HandleDeath()
{
	if (GetHealth() <= 0.0f)
	{
		if (ACoopPlayerState* PS = Cast<ACoopPlayerState>(GetOwningActor()))
		{
			PS->DeathState();
		}
	}
}
