// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/Movement/GA_CustomMovement.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"

UGA_CustomMovement::UGA_CustomMovement()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_CustomMovement::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    PreExecuteAbility(Handle, ActorInfo, ActivationInfo);

    if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
    {
        ExecuteGameplayCue(Character, ActorInfo);

        CustomMovementActivation(Character, ActivationInfo);
    }
}

void UGA_CustomMovement::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
    {
        CustomMovementActivation(Character, ActivationInfo, true);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_CustomMovement::PreExecuteAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
}

void UGA_CustomMovement::ExecuteGameplayCue(ACharacter* Character, const FGameplayAbilityActorInfo* ActorInfo)
{
    if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
    {
        if (MovementCueTag.IsValid())
        {
            FGameplayCueParameters CueParameters;
            CueParameters.Instigator = Character;

            ASC->ExecuteGameplayCue(MovementCueTag, CueParameters);
        }
    }
}

void UGA_CustomMovement::CustomMovementActivation(ACharacter* Character, const FGameplayAbilityActivationInfo ActivationInfo, bool IsEndAbility)
{
    if (Character->IsLocallyControlled() || HasAuthority(&ActivationInfo))
    {
        if (UCoop_CharacterMovementComponent* MovementComponent = Cast<UCoop_CharacterMovementComponent>(Character->GetCharacterMovement()))
        {
            EMovementAction MovementAction =  !IsEndAbility ? MovementActionToTrigger : EMovementAction::None;
            
            MovementComponent->SetCustomMovementAction(MovementAction);
        }
    }
}
