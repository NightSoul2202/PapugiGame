// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Reload.generated.h"

class UAmmoComponent;
class UGameplayEffect;
class ABaseEquipment;

UCLASS()
class PAPUGIGAME_API UGA_Reload : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Reload();

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coop|Reload")
	TSubclassOf<UGameplayEffect> ReloadEffectClass;

	UFUNCTION(BlueprintCallable, Category = "Coop|Reload")
	UFirearmComponent* GetFirearmComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Coop|Reload")
	UAmmoComponent* GetWeaponAmmoComponent() const;

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageCancelled();

	UFUNCTION()
	void OnReloadEventReceived(FGameplayEventData EventData);

private:
	ABaseEquipment* GetEquippedItem() const;
};
