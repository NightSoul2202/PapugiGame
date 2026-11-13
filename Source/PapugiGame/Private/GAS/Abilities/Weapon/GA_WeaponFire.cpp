// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/Weapon/GA_WeaponFire.h"
#include "Equipment/Components/AmmoComponent.h"
#include "GameplayEffect.h"
#include "Equipment/Components/FirearmComponent.h"
#include "Components/Equipment/EquipmentComponent.h"
#include "Equipment/Core/BaseEquipment.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"


UGA_WeaponFire::UGA_WeaponFire()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

    AmmoCost = 1;
    LastFireTime = 0.0f;
    CurrentBurstShotsFired = 0;
}

bool UGA_WeaponFire::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        return false;
    }

    UFirearmComponent* FirearmComp = GetFirearmComponent();
    if (!FirearmComp)
    {
        return false;
    }

    float FireRate = GetFireRate();
   
    if (!CanShotAfterLastFire(FireRate))
    {
        return false;
    }

    return true;
}

void UGA_WeaponFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
    }

    UFirearmComponent* FirearmComp = GetFirearmComponent();
    if (!FirearmComp)
    {
        return;
    }

    CurrentBurstShotsFired = 0;

    OnFireTick();

    if (FirearmComp->CurrentFireMode == EFireMode::Single)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    float FireRate = GetFireRate();

    SetAutoFireTimer(FireRate);
}

void UGA_WeaponFire::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_WeaponFire::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    Super::InputReleased(Handle, ActorInfo, ActivationInfo);   

    UFirearmComponent* FirearmComponent = GetFirearmComponent();

    if (FirearmComponent && FirearmComponent->CurrentFireMode == EFireMode::Auto)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
    }
}

UAmmoComponent* UGA_WeaponFire::GetWeaponAmmoComponent() const
{
    ABaseEquipment* CurrentItem = GetEquippedItem();
   
    return (CurrentItem) ? CurrentItem->FindComponentByClass<UAmmoComponent>() : nullptr;
}

UFirearmComponent* UGA_WeaponFire::GetFirearmComponent() const
{
    ABaseEquipment* CurrentItem = GetEquippedItem();

    return (CurrentItem) ? CurrentItem->FindComponentByClass<UFirearmComponent>() : nullptr;
}

bool UGA_WeaponFire::PerformHitTrace(FHitResult& OutHitResult, float InWeaponRange) const
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

    FVector TraceEnd = TraceStart + (ViewRotation.Vector() * InWeaponRange);

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

void UGA_WeaponFire::ApplyDamageToTarget(AActor* TargetActor, float BaseDamage) const
{
    if (!TargetActor || !DamageEffectClass || !CurrentActorInfo)
    {
        return;
    }

    UAbilitySystemComponent* SourceASC = CurrentActorInfo->AbilitySystemComponent.Get();
    UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);;

    if (SourceASC && TargetASC)
    {
        FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
        EffectContext.AddInstigator(CurrentActorInfo->AvatarActor.Get(), CurrentActorInfo->AvatarActor.Get());

        FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, EffectContext);

        if (SpecHandle.IsValid())
        {
            FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag("Data.Damage");
            SpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageTag, BaseDamage);

            SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
        }
    }
}

void UGA_WeaponFire::OnFireTick()
{
    LastFireTime = GetWorld()->GetTimeSeconds();

    UFirearmComponent* FirearmComponent = GetFirearmComponent();
    UAmmoComponent* AmmoComponent = GetWeaponAmmoComponent();

    if (!AmmoComponent || !AmmoComponent->TryConsumeAmmo(AmmoCost) || !FirearmComponent)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    ProcessHit(FirearmComponent);

    if (FirearmComponent->CurrentFireMode == EFireMode::Burst)
    {
        HandleBurstLogic(FirearmComponent);
    }
}

ABaseEquipment* UGA_WeaponFire::GetEquippedItem() const
{
    AActor* Avatar = CurrentActorInfo->AvatarActor.Get();
    if (UEquipmentComponent* EquipComp = Avatar->FindComponentByClass<UEquipmentComponent>())
    {
        if (ABaseEquipment* BaseEquip = EquipComp->GetCurrentItem())
        {
            return BaseEquip;
        }
    }

    return nullptr;
}

float UGA_WeaponFire::GetFireRate() const
{
    return 60.0f / (float)FMath::Max(0.0f, GetFirearmComponent()->RoundsPerMinute);
}

bool UGA_WeaponFire::CanShotAfterLastFire(float FireRate) const
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if ((CurrentTime - LastFireTime) < FireRate)
    {
        return false;
    }

    return true;
}

void UGA_WeaponFire::SetAutoFireTimer(float FireRate)
{
    GetWorld()->GetTimerManager().SetTimer
    (
        FireTimerHandle,
        this,
        &UGA_WeaponFire::OnFireTick,
        FireRate,
        true
    );
}

void UGA_WeaponFire::HandleBurstLogic(UFirearmComponent* FirearmComponent)
{
    CurrentBurstShotsFired++;
    if (CurrentBurstShotsFired >= FirearmComponent->ShotsPerBurst)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
    }
}

void UGA_WeaponFire::ProcessHit(UFirearmComponent* FirearmComponent)
{
    FHitResult HitResult;
    if (PerformHitTrace(HitResult, FirearmComponent->WeaponRange))
    {
        if (HitResult.GetActor() && HasAuthority(&CurrentActivationInfo))
        {
            ApplyDamageToTarget(HitResult.GetActor(), FirearmComponent->BaseDamage);
        }
    }
}
