	// Fill out your copyright notice in the Description page of Project Settings.

	#pragma once

	#include "CoreMinimal.h"
	#include "Abilities/GameplayAbility.h"
	#include "GameplayEffect.h"
	#include "GA_MeleeAttack.generated.h"

	class ABaseEquipment;
	class UMeleeComponent;

	UCLASS()
	class PAPUGIGAME_API UGA_MeleeAttack : public UGameplayAbility
	{
		GENERATED_BODY()

	public:
		UGA_MeleeAttack();

	protected:
		virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
		virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
		virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
		virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

		UFUNCTION()
		void OnComboWindowReceived(FGameplayEventData EventData);

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coop|Melee")
		TSubclassOf<UGameplayEffect> MeleeEffectClass;

		UFUNCTION(BlueprintCallable, Category = "Coop|Melee")
		UMeleeComponent* GetMeleeComponent() const;

		UFUNCTION()
		void OnMontageCompleted();

		UFUNCTION()
		void OnMontageCancelled();

		UFUNCTION()
		void OnHitEventReceived(FGameplayEventData EventData);

	private:
		ABaseEquipment* GetEquippedItem() const;

		TSet<AActor*> HitTargets;

		void ProcessHit();

		int32 ComboIndex = 1;
		bool bComboSaved = false;
	};
