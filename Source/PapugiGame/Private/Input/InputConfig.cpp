// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/InputConfig.h"
#include "InputAction.h"

FGameplayTag UInputConfig::FindAbilityTagForInputAction(const UInputAction* InputAction, bool bLogNotFound) const
{
	for (const FCoop_InputAction& Info : InputActions)
	{
		if (Info.InputAction && Info.InputAction == InputAction)
		{
			return Info.InputTag;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Not found GameplayTag for InputAction [%s] in InputConfig"), *InputAction->GetName());
	}

	return FGameplayTag();
}
