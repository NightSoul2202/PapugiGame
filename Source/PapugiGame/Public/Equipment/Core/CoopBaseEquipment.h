// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"
#include "Interfaces/CoopInteractable.h"
#include "CoopBaseEquipment.generated.h"

class UBoxComponent;
class USkeletalMeshComponent;
class UAbilitySystemComponent;
class UGameplayAbility;
class UGameplayEffect;

UCLASS()
class PAPUGIGAME_API ACoopBaseEquipment : public AActor, public ICoopInteractable
{
	GENERATED_BODY()
	
public:
	ACoopBaseEquipment();

	virtual void EquipItem(UAbilitySystemComponent* TargetASC, USkeletalMeshComponent* TargetMesh);
	virtual void UnequipItem(UAbilitySystemComponent* TargetASC);

	virtual void Interact_Implementation(AActor* InstigatorActor) override;

	UFUNCTION(BlueprintPure, Category = "Coop|Equipment")
	USkeletalMeshComponent* GetEquipmentMesh() const;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coop|Equipment")
	TObjectPtr<UBoxComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coop|Equipment")
	TObjectPtr<USkeletalMeshComponent> EquipmentMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Coop|Equipment")
	FName AttachmentSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Coop|GAS")
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Coop|GAS")
	TArray<TSubclassOf<UGameplayEffect>> GrantedEffects;

	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;
	TArray<FActiveGameplayEffectHandle> GrantedEffectHandles;

	void AddGrantedAbilities(UAbilitySystemComponent* TargetASC);
	void AddGrantedEffects(UAbilitySystemComponent* TargetASC);
	void RemoveGrantedAbilities(UAbilitySystemComponent* TargetASC);
	void RemoveGrantedEffects(UAbilitySystemComponent* TargetASC);
};
