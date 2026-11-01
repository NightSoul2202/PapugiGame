// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/GA_WeaponFire.h"
#include "Equipment/Components/CoopAmmoComponent.h"
#include "GameplayEffect.h"
#include "Components/Equipment/CoopEquipmentComponent.h"
#include "Equipment/Core/CoopBaseEquipment.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"


UGA_WeaponFire::UGA_WeaponFire()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	WeaponRange = 1000.0f;
    AmmoCost = 1;
}

void UGA_WeaponFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
    }

	UCoopAmmoComponent* AmmoComponent = GetWeaponAmmoComponent();
    if (!AmmoComponent)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
    }

    if (!AmmoComponent->TryConsumeAmmo(AmmoCost))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

	FHitResult HitResult;
    if (PerformHitTrace(HitResult))
    {
        if (HitResult.GetActor() && HasAuthority(&ActivationInfo))
        {
            ApplyDamageToTarget(HitResult.GetActor());
		}
    }

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

UCoopAmmoComponent* UGA_WeaponFire::GetWeaponAmmoComponent() const
{
    if (!CurrentActorInfo || !CurrentActorInfo->AvatarActor.IsValid())
    {
        return nullptr;
	}

	AActor* AvatarActor = CurrentActorInfo->AvatarActor.Get();

    if (UCoopEquipmentComponent* EquipComponent = AvatarActor->FindComponentByClass<UCoopEquipmentComponent>())
    {
        if (ACoopBaseEquipment* CurrentItem = EquipComponent->GetCurrentItem())
        {
			return CurrentItem->FindComponentByClass<UCoopAmmoComponent>(); 
		}
    }
        
    return nullptr;
}

bool UGA_WeaponFire::PerformHitTrace(FHitResult& OutHitResult) const
{
    if (!CurrentActorInfo || !CurrentActorInfo->PlayerController.IsValid())
    {
        return false;
	}

	APawn* Pawn = Cast<APawn>(CurrentActorInfo->AvatarActor.Get());
    if (!Pawn)
    {
        return false;
    }

    AController* Controller = Pawn->GetController();
    if (!Controller)
    {
        return false;
    }

    FVector TraceStart;
    FRotator ViewRotation;
    Controller->GetPlayerViewPoint(TraceStart, ViewRotation);

    FVector TraceEnd = TraceStart + (ViewRotation.Vector() * WeaponRange);

    FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponFireTrace), true, Pawn);
    TraceParams.bReturnPhysicalMaterial = true;

    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }
    
    bool bHit = World->LineTraceSingleByChannel
    (
        OutHitResult,
        TraceStart,
        TraceEnd,
        ECC_Visibility,
        TraceParams
    );

#if WITH_EDITOR
    UKismetSystemLibrary::DrawDebugLine(World, TraceStart, bHit ? OutHitResult.ImpactPoint : TraceEnd, FLinearColor::Red, 2.0f, 1.0f);
#endif

    return bHit;
}

void UGA_WeaponFire::ApplyDamageToTarget(AActor* TargetActor) const
{
    if (!TargetActor || !DamageEffectClass || !CurrentActorInfo)
    {
        return;
    }

    UAbilitySystemComponent* SourceASC = CurrentActorInfo->AbilitySystemComponent.Get();
    UAbilitySystemComponent* TargetASC = TargetActor->FindComponentByClass<UAbilitySystemComponent>();

    if (SourceASC && TargetASC)
    {
        FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
        EffectContext.AddInstigator(CurrentActorInfo->AvatarActor.Get(), CurrentActorInfo->AvatarActor.Get());

        FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, EffectContext);

        if (SpecHandle.IsValid())
        {
            SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
        }
    }
}
