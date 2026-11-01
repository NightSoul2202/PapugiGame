// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CoopAmmoComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, CurrentAmmo, int32, MaxAmmo);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PAPUGIGAME_API UCoopAmmoComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCoopAmmoComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coop|Ammo")
	int32 MaxAmmo = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coop|Ammo", ReplicatedUsing = OnRep_CurrentAmmo)
	int32 CurrentAmmo;

	UFUNCTION(BlueprintCallable, Category = "Coop|Ammo")
	bool TryConsumeAmmo(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Coop|Ammo")
	void OnRep_CurrentAmmo(int32 OldAmmo);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable, Category = "Coop|Ammo")
	FOnAmmoChanged OnAmmoChanged;
			
};
