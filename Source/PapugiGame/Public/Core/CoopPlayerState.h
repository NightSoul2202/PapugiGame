// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "CoopPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnOutHealth);

class UAbilitySystemComponent;
class UCoopAttributeSet;

UCLASS()
class PAPUGIGAME_API ACoopPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACoopPlayerState();

	FOnOutHealth OnOutHealthDelegate;

	void DeathState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UCoopAttributeSet> AttributeSet;
	
};
