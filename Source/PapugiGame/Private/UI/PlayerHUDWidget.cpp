// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHUDWidget.h"

void UPlayerHUDWidget::UpdateAmmoData(int32 InCurrentAmmo, int32 InMaxAmmo)
{
	CurrentAmmo = InCurrentAmmo;
	MaxAmmo = InMaxAmmo;
}

void UPlayerHUDWidget::UpdateHealth(float InHealth, float InMaxHealth)
{
	CurrentHealth = InHealth;
	MaxHealth = InMaxHealth;
}
