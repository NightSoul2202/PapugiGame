// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HeroClassInfo.generated.h"

class UGameplayEffect;
class UGameplayAbility;
class ABaseEquipment;

UCLASS()
class PAPUGIGAME_API UHeroClassInfo : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Class Info")
	FName ClassName;

	UPROPERTY(EditDefaultsOnly, Category = "Class Info")
	FString ClassDescription;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	TSubclassOf<UGameplayEffect> BaseStatsEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> ClassAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TSubclassOf<ABaseEquipment> StartingWeapon;
};
