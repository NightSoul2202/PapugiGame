// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeleeComponent.generated.h"

class UAnimMontage;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PAPUGIGAME_API UMeleeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMeleeComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coop|Melee Stats")
	float BaseDamage = 25.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coop|Melee Stats")
	float TraceDistance = 150.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coop|Melee Stats")
	float TraceRadius = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Coop|Melee Stats")
	UAnimMontage* MeleeAttackMontage;


		
};
