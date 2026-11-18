// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/HeroInit.h"
#include "EquipmentComponent.generated.h"

class ABaseEquipment;
class UAbilitySystemComponent;
class USkeletalMeshComponent;
class UHeroClassInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentChanged, ABaseEquipment*, NewItem);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PAPUGIGAME_API UEquipmentComponent : public UActorComponent, public IHeroInit
{
	GENERATED_BODY()

public:	
	UEquipmentComponent();

	UFUNCTION(BlueprintCallable, Category = "Coop|Equipment")
	void EquipItem(ABaseEquipment* ItemToEquip);

	UFUNCTION(BlueprintCallable, Category = "Coop|Equipment")
	void DropItem();

	UFUNCTION(BlueprintPure, Category = "Coop|Equipment")
	ABaseEquipment* GetCurrentItem() const;
		
	UPROPERTY(BlueprintAssignable, Category = "Coop|Equipment")
	FOnEquipmentChanged OnEquipmentChanged;

	void OnHeroInitialized(UHeroClassInfo* ClassInfo) override;

protected:
	virtual void BeginPlay() override;	
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Category = "Coop|Equipment", ReplicatedUsing = OnRep_CurrentItem)
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
