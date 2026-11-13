// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MovementAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class PAPUGIGAME_API UMovementAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UMovementAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_SpeedSprint)
	FGameplayAttributeData SpeedSprint;
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, SpeedSprint)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxSpeedSprint)
	FGameplayAttributeData MaxSpeedSprint;
	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, MaxSpeedSprint)

protected:
	UFUNCTION()
	void OnRep_SpeedSprint(const FGameplayAttributeData& OldSpeed);

	UFUNCTION()
	void OnRep_MaxSpeedSprint(const FGameplayAttributeData& MaxOldSpeed);
	
};
