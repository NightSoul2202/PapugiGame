// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "Characters/HeroClassInfo.h"
#include "Coop_PlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerClassUpdated, UHeroClassInfo*, NewClassData);

class UAbilitySystemComponent;
class UHealthAttributeSet;
class UMovementAttributeSet;

UCLASS()
class PAPUGIGAME_API ACoop_PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACoop_PlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UHeroClassInfo* GetHeroClassData() const;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerClassUpdated OnPlayerClassUpdatedDelegate;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coop|Abilities")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coop|Attributes")
	TObjectPtr<UHealthAttributeSet> HealthAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coop|Attributes")
	TObjectPtr<UMovementAttributeSet> MovementAttributeSet;

	UPROPERTY(Replicated)
	TObjectPtr<UHeroClassInfo> SelectedClassData;

	UFUNCTION()
	virtual void OnRep_SelectedClass();
	
};
