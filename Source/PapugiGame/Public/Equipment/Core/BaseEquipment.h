// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"
#include "Interfaces/Interactable.h"
#include "BaseEquipment.generated.h"

class UBoxComponent;
class USkeletalMeshComponent;
class UAbilitySystemComponent;
class UGameplayAbility;
class UGameplayEffect;

UCLASS()
class PAPUGIGAME_API ABaseEquipment : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:
	ABaseEquipment();

	virtual void EquipItem(UAbilitySystemComponent* TargetASC, USkeletalMeshComponent* TargetMesh);
	virtual void UnequipItem(UAbilitySystemComponent* TargetASC);

	virtual void Interact_Implementation(AActor* InstigatorActor) override;

	UFUNCTION(BlueprintPure, Category = "|Equipment")
	USkeletalMeshComponent* GetEquipmentMesh() const;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "|Equipment")
	TObjectPtr<UBoxComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "|Equipment")
	TObjectPtr<USkeletalMeshComponent> EquipmentMesh;

	UPROPERTY(EditDefaultsOnly, Category = "|Equipment")
	FName AttachmentSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "|GAS")
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "|GAS")
	TArray<TSubclassOf<UGameplayEffect>> GrantedEffects;

	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;
	TArray<FActiveGameplayEffectHandle> GrantedEffectHandles;

	void AddGrantedAbilities(UAbilitySystemComponent* TargetASC);
	void AddGrantedEffects(UAbilitySystemComponent* TargetASC);
	void RemoveGrantedAbilities(UAbilitySystemComponent* TargetASC);
	void RemoveGrantedEffects(UAbilitySystemComponent* TargetASC);
};
