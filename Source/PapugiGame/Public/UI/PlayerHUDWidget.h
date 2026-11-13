// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"


UCLASS()
class PAPUGIGAME_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateAmmoData(int32 InCurrentAmmo, int32 InMaxAmmo);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnWeaponChanged(bool bIsEquipped);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	int32 CurrentAmmo = 0;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	int32 MaxAmmo = 0;
};
