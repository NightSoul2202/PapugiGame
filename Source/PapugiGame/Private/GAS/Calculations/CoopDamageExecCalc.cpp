// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Calculations/CoopDamageExecCalc.h"
#include "GAS/Attributes/CoopAttributeSet.h"
#include "AbilitySystemComponent.h"

struct CoopDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);

	CoopDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UCoopAttributeSet, Armor, Target, false);
	}
};

static const CoopDamageStatics& DamageStatics()
{
	static CoopDamageStatics Statics;
	return Statics;
}


UCoopDamageExecCalc::UCoopDamageExecCalc()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}

void UCoopDamageExecCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
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
			UCoopAttributeSet::GetIncomingDamageAttribute(),
			EGameplayModOp::Additive,
			FinalDamage
		));
	}
}
