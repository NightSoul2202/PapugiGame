// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "CoopDamageExecCalc.generated.h"

UCLASS()
class PAPUGIGAME_API UCoopDamageExecCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public: 
	UCoopDamageExecCalc();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
