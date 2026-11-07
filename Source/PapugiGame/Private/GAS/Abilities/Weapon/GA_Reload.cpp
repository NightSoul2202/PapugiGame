// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/Weapon/GA_Reload.h"
#include "Equipment/Core/BaseEquipment.h"
#include "Equipment/Components/AmmoComponent.h"
#include "Equipment/Components/FirearmComponent.h"
#include "Components/Equipment/EquipmentComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"

UGA_Reload::UGA_Reload()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UGA_Reload::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	UAmmoComponent* AmmoComp = GetWeaponAmmoComponent();
	if (!AmmoComp)
	{
		return false;
	}

	return true;
}

void UGA_Reload::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UFirearmComponent* FirearmComp = GetFirearmComponent();
	if (!FirearmComp || !FirearmComp->ReloadMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, FirearmComp->ReloadMontage, 1.0f);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_Reload::OnMontageCompleted);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_Reload::OnMontageCancelled);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_Reload::OnMontageCancelled);
	MontageTask->ReadyForActivation();

	FGameplayTag ReloadTag = FGameplayTag::RequestGameplayTag(FName("Event.Weapon.ReloadAmmo"));

	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ReloadTag);
	EventTask->EventReceived.AddDynamic(this, &UGA_Reload::OnReloadEventReceived);
	EventTask->ReadyForActivation();
}

UAmmoComponent* UGA_Reload::GetWeaponAmmoComponent() const
{
	ABaseEquipment* BaseEquip = GetEquippedItem();
	return (BaseEquip) ? BaseEquip->FindComponentByClass<UAmmoComponent>() : nullptr;
}

UFirearmComponent* UGA_Reload::GetFirearmComponent() const
{
	ABaseEquipment* BaseEquip = GetEquippedItem();
	return (BaseEquip) ? BaseEquip->FindComponentByClass<UFirearmComponent>() : nullptr;
}

void UGA_Reload::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Reload::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Reload::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_Reload::OnReloadEventReceived(FGameplayEventData EventData)
{
	if (UAmmoComponent* AmmoComp = GetWeaponAmmoComponent())
	{
		AmmoComp->ReloadAmmo();
	}
}

ABaseEquipment* UGA_Reload::GetEquippedItem() const
{
	if (!CurrentActorInfo || !CurrentActorInfo->AvatarActor.IsValid())
	{
		return nullptr;
	}

	AActor* Avatar = CurrentActorInfo->AvatarActor.Get();

	if (UEquipmentComponent* EquipComp = Avatar->FindComponentByClass<UEquipmentComponent>())
	{
		return EquipComp->GetCurrentItem();
	}

	return nullptr;
}
