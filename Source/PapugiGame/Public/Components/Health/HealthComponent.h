// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);

class UAbilitySystemComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PAPUGIGAME_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UHealthComponent();

	UPROPERTY(BlueprintAssignable, Category = "Coop|Health")
	FOnCharacterDeathSignature OnCharacterDeathDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Coop|Health")
	FOnHealthChanged OnHealthChangedDelegate;

	void InitializeHealthComponent(UAbilitySystemComponent* ASC);

protected: 
	virtual void OnHealthChanged(const FOnAttributeChangeData& Data);

private:
	void InitHealth(UAbilitySystemComponent* ASC);
};
