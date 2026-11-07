// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FirearmComponent.generated.h"

class UAnimMontage;

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	Single,
	Burst,
	Auto
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PAPUGIGAME_API UFirearmComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFirearmComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "|Firearm")
	float WeaponRange = 10000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "|Firearm")
	int32 RoundsPerMinute = 600;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "|Firearm")
	int32 ShotsPerBurst = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "|Firearm")
	TArray<EFireMode> SupportedFireModes;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "|Firearm")
	EFireMode CurrentFireMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "|Firearm")
	UAnimMontage* ReloadMontage;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SwitchFireMode();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
		
};
