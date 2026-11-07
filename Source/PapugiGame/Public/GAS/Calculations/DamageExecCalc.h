// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "DamageExecCalc.generated.h"

UCLASS()
class PAPUGIGAME_API UDamageExecCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public: 
	UDamageExecCalc();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
