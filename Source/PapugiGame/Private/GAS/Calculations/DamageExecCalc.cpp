// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Calculations/DamageExecCalc.h"
#include "GAS/Attributes/Coop_AttributeSet.h"
#include "AbilitySystemComponent.h"

struct Coop__DamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);

	Coop__DamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCoop_AttributeSet, Armor, Target, false);
	}
};

static const Coop__DamageStatics& DamageStatics()
{
	static Coop__DamageStatics Statics;
	return Statics;
}


UDamageExecCalc::UDamageExecCalc()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}

void UDamageExecCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	if (!SourceASC || !TargetASC)
	{
		return;
	}

	FGameplayTag InvulnerableTag = FGameplayTag::RequestGameplayTag(FName("State.Buff.Invulnerable"));
	if (InvulnerableTag.IsValid() && TargetASC->HasMatchingGameplayTag(InvulnerableTag))
	{
		return;
	}

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float BaseDamage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), false, 0.0f);

	float Armor = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParameters, Armor);
	Armor = FMath::Max<float>(Armor, 0.0f);

	float FinalDamage = BaseDamage * (1.0f - (Armor / 100.f));

	FinalDamage = FMath::Max<float>(FinalDamage, 0.0f);

	if (FinalDamage > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
			UCoop_AttributeSet::GetIncomingDamageAttribute(),
			EGameplayModOp::Additive,
			FinalDamage
		));
	}
}
