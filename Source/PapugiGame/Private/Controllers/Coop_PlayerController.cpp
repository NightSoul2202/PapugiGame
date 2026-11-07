// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/Coop_PlayerController.h"
#include "UI/PlayerHUDWidget.h"
#include "Characters/Coop_PlayerCharacter.h"
#include "Components/Equipment/EquipmentComponent.h"
#include "Equipment/Components/AmmoComponent.h"
#include "Equipment/Core/BaseEquipment.h"

void ACoop_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() && HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UPlayerHUDWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();
			HUDWidgetInstance->OnWeaponChanged(false);
		}
	}
}

void ACoop_PlayerController::SetPawn(APawn* InPawn)
{	
	Super::SetPawn(InPawn);

	if (IsLocalController() && InPawn)
	{
		if (UEquipmentComponent* EquipComp = InPawn->FindComponentByClass<UEquipmentComponent>())
		{
			EquipComp->OnEquipmentChanged.RemoveDynamic(this, &ACoop_PlayerController::HandleEquipmentChanged);
			EquipComp->OnEquipmentChanged.AddDynamic(this, &ACoop_PlayerController::HandleEquipmentChanged);

			HandleEquipmentChanged(EquipComp->GetCurrentItem());
		}
	}
}

void ACoop_PlayerController::HandleEquipmentChanged(ABaseEquipment* NewEquipment)
{
	if (!HUDWidgetInstance)
	{
		return;
	}

	if (NewEquipment)
	{
		if (UAmmoComponent* AmmoComp = NewEquipment->FindComponentByClass<UAmmoComponent>())
		{
			AmmoComp->OnAmmoChanged.RemoveDynamic(this, &ACoop_PlayerController::HandleAmmoChanged);
			AmmoComp->OnAmmoChanged.AddDynamic(this, &ACoop_PlayerController::HandleAmmoChanged);

			HandleAmmoChanged(AmmoComp->CurrentAmmo, AmmoComp->MaxAmmo);
		}
	}
	else
	{
		HUDWidgetInstance->OnWeaponChanged(false);

		HandleAmmoChanged(0, 0);
	}
}

void ACoop_PlayerController::HandleAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->UpdateAmmoData(CurrentAmmo, MaxAmmo);
	}
}
