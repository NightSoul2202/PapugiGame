// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/Components/CoopAmmoComponent.h"
#include "Net/UnrealNetwork.h"

UCoopAmmoComponent::UCoopAmmoComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	CurrentAmmo = MaxAmmo;
}

bool UCoopAmmoComponent::TryConsumeAmmo(int32 Amount)
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

void UCoopAmmoComponent::OnRep_CurrentAmmo(int32 OldAmmo)
{
	if (CurrentAmmo != OldAmmo)
	{
		OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);
	}
}

void UCoopAmmoComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UCoopAmmoComponent, CurrentAmmo, COND_OwnerOnly);
}


