// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CoopEquipmentComponent.generated.h"

class ACoopBaseEquipment;
class UAbilitySystemComponent;
class USkeletalMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentChanged, ACoopBaseEquipment*, NewItem);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PAPUGIGAME_API UCoopEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCoopEquipmentComponent();

	UFUNCTION(BlueprintCallable, Category = "Coop|Equipment")
	void EquipItem(ACoopBaseEquipment* ItemToEquip);

	UFUNCTION(BlueprintCallable, Category = "Coop|Equipment")
	void DropItem();

	UFUNCTION(BlueprintPure, Category = "Coop|Equipment")
	ACoopBaseEquipment* GetCurrentItem() const;
		
	UPROPERTY(BlueprintAssignable, Category = "Coop|Equipment")
	FOnEquipmentChanged OnEquipmentChanged;

protected:
	virtual void BeginPlay() override;	
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Category = "Coop|Equipment", ReplicatedUsing = OnRep_CurrentItem)
	TObjectPtr<ACoopBaseEquipment> CurrentItem;

	UFUNCTION()
	void OnRep_CurrentItem(ACoopBaseEquipment* OldItem);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EquipItem(ACoopBaseEquipment* ItemToEquip);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DropItem();	
	
private:
	UAbilitySystemComponent* GetOwnerASC(AActor* Owner);
	USkeletalMeshComponent* GetOwnerMesh(AActor* Owner);
};
