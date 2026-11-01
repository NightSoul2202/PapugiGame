// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "CoopInputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FCoopInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InputTag;
};

UCLASS()
class PAPUGIGAME_API UCoopInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coop|Input")
	TArray<FCoopInputAction> InputActions;

	FGameplayTag FindAbilityTagForInputAction(const UInputAction* InputAction, bool bLogNotFound = false) const;
	
};
