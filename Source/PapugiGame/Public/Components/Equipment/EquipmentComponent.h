// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"

class ABaseEquipment;
class UAbilitySystemComponent;
class USkeletalMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentChanged, ABaseEquipment*, NewItem);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PAPUGIGAME_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEquipmentComponent();

	UFUNCTION(BlueprintCallable, Category = "|Equipment")
	void EquipItem(ABaseEquipment* ItemToEquip);

	UFUNCTION(BlueprintCallable, Category = "|Equipment")
	void DropItem();

	UFUNCTION(BlueprintPure, Category = "|Equipment")
	ABaseEquipment* GetCurrentItem() const;
		
	UPROPERTY(BlueprintAssignable, Category = "|Equipment")
	FOnEquipmentChanged OnEquipmentChanged;

protected:
	virtual void BeginPlay() override;	
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Category = "|Equipment", ReplicatedUsing = OnRep_CurrentItem)
	TObjectPtr<ABaseEquipment> CurrentItem;

	UFUNCTION()
	void OnRep_CurrentItem(ABaseEquipment* OldItem);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EquipItem(ABaseEquipment* ItemToEquip);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DropItem();	
	
private:
	UAbilitySystemComponent* GetOwnerASC(AActor* Owner);
	USkeletalMeshComponent* GetOwnerMesh(AActor* Owner);
};
