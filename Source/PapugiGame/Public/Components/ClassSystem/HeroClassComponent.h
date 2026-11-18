// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HeroClassComponent.generated.h"

class UHeroClassInfo;
class UAbilitySystemComponent;
class ACoop_PlayerState;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PAPUGIGAME_API UHeroClassComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHeroClassComponent();

	void InitializeClass();

private:

	void ApplyEffects(UHeroClassInfo* ClassData, UAbilitySystemComponent* ASC, AActor* Actor);

	void InitOnComponents(TArray<UActorComponent*> AllComponents, ACoop_PlayerState* PS);
};
