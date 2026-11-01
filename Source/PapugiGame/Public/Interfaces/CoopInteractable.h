// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CoopInteractable.generated.h"

class AActor;

UINTERFACE(MinimalAPI, Blueprintable)
class UCoopInteractable : public UInterface
{
	GENERATED_BODY()
};


class PAPUGIGAME_API ICoopInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Coop|Interaction")
	void Interact(AActor* Instigator);
};
