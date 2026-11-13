// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Coop_PlayerController.generated.h"

UCLASS()
class PAPUGIGAME_API ACoop_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	virtual void SetPawn(APawn* InPawn) override;

	UPROPERTY(EditDefaultsOnly, Category = "Coop|UI")
	TSubclassOf<class UPlayerHUDWidget> HUDWidgetClass;
		
	UPROPERTY()
	TObjectPtr<UPlayerHUDWidget> HUDWidgetInstance;

	UFUNCTION()
	void HandleEquipmentChanged(class ABaseEquipment* NewEquipment);

	UFUNCTION()
	void HandleAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo);
};
