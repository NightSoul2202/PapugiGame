// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CoopAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class PAPUGIGAME_API UCoopAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UCoopAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UCoopAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UCoopAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_SpeedSprint)
	FGameplayAttributeData SpeedSprint;
	ATTRIBUTE_ACCESSORS(UCoopAttributeSet, SpeedSprint)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxSpeedSprint)
	FGameplayAttributeData MaxSpeedSprint;
	ATTRIBUTE_ACCESSORS(UCoopAttributeSet, MaxSpeedSprint)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UCoopAttributeSet, Armor)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UCoopAttributeSet, IncomingDamage)

protected:

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	void OnRep_SpeedSprint(const FGameplayAttributeData& OldSpeed);

	UFUNCTION()
	void OnRep_MaxSpeedSprint(const FGameplayAttributeData& MaxOldSpeed);
	
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor);

private:

	void PerformIncomingDamage();
	void HandleDeath();
};
