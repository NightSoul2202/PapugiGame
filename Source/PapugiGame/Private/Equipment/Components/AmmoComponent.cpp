// Fill out your copyright notice in the Description page of Project Settings.

#include "Equipment/Components/AmmoComponent.h"
#include "Net/UnrealNetwork.h"

UAmmoComponent::UAmmoComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	CurrentAmmo = MaxAmmo;
}

bool UAmmoComponent::TryConsumeAmmo(int32 Amount)
{
	if (CurrentAmmo > 0)
	{
		CurrentAmmo -= Amount;

		if (GetOwner()->HasAuthority() || GetOwner()->GetInstigatorController()->IsLocalPlayerController())
		{
			OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
		}
		return true;
	}
	return false;
}

void UAmmoComponent::OnRep_CurrentAmmo(int32 OldAmmo)
{
	if (CurrentAmmo != OldAmmo)
	{
		OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
	}
}

void UAmmoComponent::ReloadAmmo()
{
	if (GetOwner()->HasAuthority())
	{
		CurrentAmmo = MaxAmmo;

		OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
	}
}

void UAmmoComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UAmmoComponent, CurrentAmmo, COND_OwnerOnly);
}


