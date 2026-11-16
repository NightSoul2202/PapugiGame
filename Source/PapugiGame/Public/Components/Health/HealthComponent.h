// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeathSignature);

class UAbilitySystemComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PAPUGIGAME_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UHealthComponent();

	UPROPERTY(BlueprintAssignable, Category = "Coop|Health")
	FOnCharacterDeathSignature OnCharacterDeathDelegate;

	void InitializeHealthComponent(UAbilitySystemComponent* ASC);

protected: 
	virtual void OnHealthChanged(const FOnAttributeChangeData& Data);
};
