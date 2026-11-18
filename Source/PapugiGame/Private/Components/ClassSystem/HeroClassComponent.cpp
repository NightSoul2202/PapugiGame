// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/ClassSystem/HeroClassComponent.h"
#include "Characters/HeroClassInfo.h"
#include "Equipment/Core/BaseEquipment.h"
#include "Components/Equipment/EquipmentComponent.h"
#include "AbilitySystemComponent.h"
#include "Core/Coop_PlayerState.h"
#include "Interfaces/HeroInit.h"

UHeroClassComponent::UHeroClassComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHeroClassComponent::InitializeClass()
{
	AActor* Actor = GetOwner();
	if (!Actor || !Actor->HasAuthority())
	{
		return;
	}

	APawn* Pawn = Cast<APawn>(Actor);
	ACoop_PlayerState* PS = Pawn ? Pawn->GetPlayerState<ACoop_PlayerState>() : nullptr;

	if (PS && PS->GetHeroClassData())
	{
		UHeroClassInfo* ClassData = PS->GetHeroClassData();
		UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();

		ApplyEffects(ClassData, ASC, Actor);

		TArray<UActorComponent*> AllComponents;
		Actor->GetComponents(AllComponents);

		InitOnComponents(AllComponents, PS);
	}
}

void UHeroClassComponent::ApplyEffects(UHeroClassInfo* ClassData, UAbilitySystemComponent* ASC, AActor* Actor)
{
	if (ASC && ClassData->BaseStatsEffect)
	{
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		Context.AddInstigator(Actor, Actor);

		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ClassData->BaseStatsEffect, 1.0f, Context);
		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void UHeroClassComponent::InitOnComponents(TArray<UActorComponent*> AllComponents, ACoop_PlayerState* PS)
{
	for (UActorComponent* ActorComp : AllComponents)
	{
		if (IHeroInit* Listener = Cast<IHeroInit>(ActorComp))
		{
			Listener->OnHeroInitialized(PS->GetHeroClassData());
		}
	}
}


