// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_WeaponFire.generated.h"

class UAmmoComponent;
class UFirearmComponent;
class UGameplayEffect;
class ABaseEquipment;

UCLASS()
class PAPUGIGAME_API UGA_WeaponFire : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_WeaponFire();

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coop|Weapon")
	int32 AmmoCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coop|Weapon")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UFUNCTION(BlueprintCallable, Category = "Coop|Weapon")
	UAmmoComponent* GetWeaponAmmoComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Coop|Weapon")
	UFirearmComponent* GetFirearmComponent() const;

	bool PerformHitTrace(FHitResult& OutHitResult, float InWeaponRange) const;

	void ApplyDamageToTarget(AActor* TargetActor, float BaseDamage) const;

private:
	FTimerHandle FireTimerHandle;

	float LastFireTime;
	int32 CurrentBurstShotsFired;

	void OnFireTick();

	ABaseEquipment* GetEquippedItem() const;
	float GetFireRate() const;
	bool CanShotAfterLastFire(float FireRate) const;

	void SetAutoFireTimer(float FireRate);

	void HandleBurstLogic(UFirearmComponent* FirearmComponent);
	void ProcessHit(UFirearmComponent* FirearmComponent);

};
