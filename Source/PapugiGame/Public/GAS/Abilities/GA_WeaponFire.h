// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_WeaponFire.generated.h"

class UCoopAmmoComponent;
class UGameplayEffect;

UCLASS()
class PAPUGIGAME_API UGA_WeaponFire : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_WeaponFire();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coop|Weapon")
	float WeaponRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coop|Weapon")
	int32 AmmoCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coop|Weapon")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UFUNCTION(BlueprintCallable, Category = "Coop|Weapon")
	UCoopAmmoComponent* GetWeaponAmmoComponent() const;

	bool PerformHitTrace(FHitResult& OutHitResult) const;

	void ApplyDamageToTarget(AActor* TargetActor) const;

	
};
