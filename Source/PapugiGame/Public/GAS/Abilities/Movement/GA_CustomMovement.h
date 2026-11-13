// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Components/Movement/Coop_CharacterMovementComponent.h"
#include "GA_CustomMovement.generated.h"

UCLASS()
class PAPUGIGAME_API UGA_CustomMovement : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_CustomMovement();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Coop|Movement Settings")
	EMovementAction MovementActionToTrigger;

	UPROPERTY(EditDefaultsOnly, Category = "Coop|Movement Settings")
	FGameplayTag MovementCueTag;

private:
	void PreExecuteAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);
	void ExecuteGameplayCue(ACharacter* Character, const FGameplayAbilityActorInfo* ActorInfo);
	void CustomMovementActivation(ACharacter* Character, const FGameplayAbilityActivationInfo ActivationInfo, bool IsEndAbility = false);
};
