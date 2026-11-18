// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/Coop_PlayerController.h"
#include "UI/PlayerHUDWidget.h"
#include "Characters/Coop_PlayerCharacter.h"
#include "Components/Equipment/EquipmentComponent.h"
#include "Components/Health/HealthComponent.h"
#include "Equipment/Components/AmmoComponent.h"
#include "Equipment/Core/BaseEquipment.h"
#include "AbilitySystemGlobals.h"
#include "GAS/Attributes/HealthAttributeSet.h"

void ACoop_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() && HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UPlayerHUDWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			if (APawn* CurrentPawn = GetPawn())
			{
				SyncHUDWithPawn(CurrentPawn);
			}
			else
			{
				HUDWidgetInstance->OnWeaponChanged(false);
				HUDWidgetInstance->OnHealthChanged();
			}
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

		if (UHealthComponent* HealthComp = InPawn->FindComponentByClass<UHealthComponent>())
		{
			HealthComp->OnHealthChangedDelegate.RemoveDynamic(this, &ACoop_PlayerController::HandleHealthChanged);
			HealthComp->OnHealthChangedDelegate.AddDynamic(this, &ACoop_PlayerController::HandleHealthChanged);

			if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InPawn))
			{
				float CurrentHealth = ASC->GetNumericAttribute(UHealthAttributeSet::GetHealthAttribute());
				float MaxHealth = ASC->GetNumericAttribute(UHealthAttributeSet::GetMaxHealthAttribute());

				HandleHealthChanged(CurrentHealth, MaxHealth);
			}
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

void ACoop_PlayerController::HandleHealthChanged(float CurrentHealth, float MaxHealth)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->UpdateHealth(CurrentHealth, MaxHealth);
	}
}

void ACoop_PlayerController::SyncHUDWithPawn(APawn* CurrentPawn)
{
	if (UEquipmentComponent* EquipComp = CurrentPawn->FindComponentByClass<UEquipmentComponent>())
	{
		HandleEquipmentChanged(EquipComp->GetCurrentItem());
	}

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CurrentPawn))
	{
		float CurrentHealth = ASC->GetNumericAttribute(UHealthAttributeSet::GetHealthAttribute());
		float MaxHealth = ASC->GetNumericAttribute(UHealthAttributeSet::GetMaxHealthAttribute());
		HandleHealthChanged(CurrentHealth, MaxHealth);
	}
}
