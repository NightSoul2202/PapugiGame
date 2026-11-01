// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/CoopInputConfig.h"
#include "InputAction.h"

FGameplayTag UCoopInputConfig::FindAbilityTagForInputAction(const UInputAction* InputAction, bool bLogNotFound) const
{
	for (const FCoopInputAction& Info : InputActions)
	{
		if (Info.InputAction && Info.InputAction == InputAction)
		{
			return Info.InputTag;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Not found GameplayTag for InputAction [%s] in CoopInputConfig"), *InputAction->GetName());
	}

	return FGameplayTag();
}
