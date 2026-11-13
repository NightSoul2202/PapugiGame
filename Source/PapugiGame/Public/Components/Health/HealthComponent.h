// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeath);

class ACoop_PlayerState;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PAPUGIGAME_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UHealthComponent();

	UPROPERTY(BlueprintAssignable, Category = "Coop|Health")
	FOnCharacterDeath OnCharacterDeathDelegate;

	void InitializeHealthComponent(ACoop_PlayerState* PS);

protected: 

	UFUNCTION()
	void OnPlayerStateDeath();
};
