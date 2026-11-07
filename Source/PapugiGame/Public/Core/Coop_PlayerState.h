// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "Coop_PlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnOutHealth);

class UAbilitySystemComponent;
class UCoop_AttributeSet;

UCLASS()
class PAPUGIGAME_API ACoop_PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACoop_PlayerState();

	FOnOutHealth OnOutHealthDelegate;

	void DeathState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UCoop_AttributeSet> AttributeSet;
	
};
