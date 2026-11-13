// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/Melee/GA_MeleeAttack.h"
#include "Equipment/Core/BaseEquipment.h"
#include "Components/Equipment/EquipmentComponent.h"
#include "Equipment/Components/MeleeComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include <Kismet/KismetSystemLibrary.h>

UGA_MeleeAttack::UGA_MeleeAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UGA_MeleeAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	UMeleeComponent* MeleeComp = GetMeleeComponent();
	if (!MeleeComp || !MeleeComp->MeleeAttackMontage)
	{
		return false;
	}

	return true;
}

void UGA_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	HitTargets.Empty();	
	ComboIndex = 1;
	bComboSaved = false;

	UMeleeComponent* MeleeComp = GetMeleeComponent();

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName("Attack1"), MeleeComp->MeleeAttackMontage, 1.0f);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_MeleeAttack::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_MeleeAttack::OnMontageCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_MeleeAttack::OnMontageCancelled);
	MontageTask->ReadyForActivation();

	FGameplayTag MeleeAttackTag = FGameplayTag::RequestGameplayTag(FName("Event.Melee.HitWindow"));
	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, MeleeAttackTag);
	EventTask->EventReceived.AddDynamic(this, &UGA_MeleeAttack::OnHitEventReceived);
	EventTask->ReadyForActivation();

	FGameplayTag ComboTag = FGameplayTag::RequestGameplayTag(FName("Event.Melee.ComboWindow"));
	UAbilityTask_WaitGameplayEvent* ComboEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ComboTag);
	ComboEventTask->EventReceived.AddDynamic(this, &UGA_MeleeAttack::OnComboWindowReceived);
	ComboEventTask->ReadyForActivation();
}

void UGA_MeleeAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_MeleeAttack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	bComboSaved = true;

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("CLICK INPUT!"));
}

void UGA_MeleeAttack::OnComboWindowReceived(FGameplayEventData EventData)
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("CLICK COMBO"));

	UMeleeComponent* MeleeComp = GetMeleeComponent();
	if (!MeleeComp)
	{
		return;
	}

	int32 TotalSections = MeleeComp->MeleeAttackMontage->CompositeSections.Num();

	if (bComboSaved && ComboIndex < TotalSections)
	{
		bComboSaved = false;

		FName CurrentSection = FName(*FString::Printf(TEXT("Attack%d"), ComboIndex));

		ComboIndex++;

		FName NextSection = FName(*FString::Printf(TEXT("Attack%d"), ComboIndex));
		MontageSetNextSectionName(CurrentSection, NextSection);

		HitTargets.Empty();
	}
}

UMeleeComponent* UGA_MeleeAttack::GetMeleeComponent() const
{
	ABaseEquipment* BaseEquip = GetEquippedItem();
	return (BaseEquip) ? BaseEquip->FindComponentByClass<UMeleeComponent>() : nullptr;
}

void UGA_MeleeAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_MeleeAttack::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_MeleeAttack::OnHitEventReceived(FGameplayEventData EventData)
{
	ProcessHit();
}

ABaseEquipment* UGA_MeleeAttack::GetEquippedItem() const
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

void UGA_MeleeAttack::ProcessHit()
{
	if (!CurrentActorInfo || !CurrentActorInfo->PlayerController.Get())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	APawn* Pawn = Cast<APawn>(CurrentActorInfo->AvatarActor.Get());
	AController* Controller = (Pawn) ? Pawn->GetController() : nullptr; 
	UMeleeComponent* MeleeComp = GetMeleeComponent();
	UWorld* World = GetWorld();

	if (!Pawn || !Controller || !MeleeComp || !World)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	FVector StartTrace;
	FRotator ViewRotation;
	Controller->GetPlayerViewPoint(StartTrace, ViewRotation);

	FVector EndTrace = StartTrace + (ViewRotation.Vector() * MeleeComp->TraceDistance);

	TArray<FHitResult> HitResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(MeleeComp->TraceRadius);
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(MeleeTrace), false, Pawn);

	bool bHit = World->SweepMultiByChannel(
		HitResults,
		StartTrace,
		EndTrace,
		FQuat::Identity,
		ECC_Visibility,
		Sphere, 
		TraceParams
	);

#if WITH_EDITOR
	UKismetSystemLibrary::DrawDebugSphere(World, EndTrace, MeleeComp->TraceRadius, 12, FLinearColor::Red, 1.0f, 1.0f);
#endif

	if (bHit && HasAuthority(&CurrentActivationInfo))
	{
		UAbilitySystemComponent* SourceASC = CurrentActorInfo->AbilitySystemComponent.Get();

		for (const FHitResult& Hit : HitResults)
		{
			AActor* TargetActor = Hit.GetActor();

			if (!TargetActor || TargetActor == Pawn|| HitTargets.Contains(TargetActor))
			{
				continue;
			}

			UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
			
			if (SourceASC && TargetASC)
			{
				FGameplayEffectContextHandle EffectHandle = SourceASC->MakeEffectContext();
				EffectHandle.AddInstigator(Pawn, Pawn);

				FGameplayEffectSpecHandle EffectSpec = SourceASC->MakeOutgoingSpec(MeleeEffectClass, 1.0f, EffectHandle);

				if (EffectSpec.IsValid())
				{
					FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName("Data.Damage"));

					EffectSpec.Data.Get()->SetSetByCallerMagnitude(DamageTag, MeleeComp->BaseDamage);

					SourceASC->ApplyGameplayEffectSpecToTarget(*EffectSpec.Data.Get(), TargetASC);
				}
				HitTargets.Add(TargetActor);
			}
		}
	}
}
