// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Equipment/CoopEquipmentComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Equipment/Core/CoopBaseEquipment.h"
#include "Components/SkeletalMeshComponent.h"

UCoopEquipmentComponent::UCoopEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);
}

void UCoopEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCoopEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCoopEquipmentComponent, CurrentItem);
}

void UCoopEquipmentComponent::OnRep_CurrentItem(ACoopBaseEquipment* OldItem)
{
	if (OldItem)
	{
		UAbilitySystemComponent* ASC = GetOwnerASC(GetOwner());
		OldItem->UnequipItem(ASC);
	}

	if (CurrentItem)
	{
		UAbilitySystemComponent* ASC = GetOwnerASC(GetOwner());
		USkeletalMeshComponent* Mesh = GetOwnerMesh(GetOwner());

		if (ASC && Mesh)
		{
			CurrentItem->EquipItem(ASC, Mesh);
		}
	}

	OnEquipmentChanged.Broadcast(CurrentItem);
}

UAbilitySystemComponent* UCoopEquipmentComponent::GetOwnerASC(AActor* Owner)
{
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner))
	{
		return ASI->GetAbilitySystemComponent();
	}
	return nullptr;
}

USkeletalMeshComponent* UCoopEquipmentComponent::GetOwnerMesh(AActor* Owner)
{
	if (!Owner)
	{
		return nullptr;
	}
	
	return Owner->FindComponentByClass<USkeletalMeshComponent>();
}

bool UCoopEquipmentComponent::Server_DropItem_Validate()
{
	return true;
}

void UCoopEquipmentComponent::Server_DropItem_Implementation()
{
	UAbilitySystemComponent* ASC = GetOwnerASC(GetOwner());

	CurrentItem->UnequipItem(ASC);

	CurrentItem = nullptr;

	OnEquipmentChanged.Broadcast(nullptr);
}

bool UCoopEquipmentComponent::Server_EquipItem_Validate(ACoopBaseEquipment* ItemToEquip)
{
	return ItemToEquip != nullptr;
}

void UCoopEquipmentComponent::Server_EquipItem_Implementation(ACoopBaseEquipment* ItemToEquip)
{
	if (CurrentItem)
	{
		Server_DropItem_Implementation();
	}

	CurrentItem = ItemToEquip;

	AActor* Owner = GetOwner();

	UAbilitySystemComponent* ItemASC = GetOwnerASC(Owner);

	USkeletalMeshComponent* ItemMesh = GetOwnerMesh(Owner);

	if (ItemASC && ItemMesh)
	{
		CurrentItem->EquipItem(ItemASC, ItemMesh);

		OnEquipmentChanged.Broadcast(CurrentItem);
	}
}

void UCoopEquipmentComponent::EquipItem(ACoopBaseEquipment* ItemToEquip)
{
	if (ItemToEquip)
	{
		Server_EquipItem(ItemToEquip);
	}
}

void UCoopEquipmentComponent::DropItem()
{
	if (CurrentItem)
	{
		Server_DropItem();
	}
}

ACoopBaseEquipment* UCoopEquipmentComponent::GetCurrentItem() const
{
	return CurrentItem;
}


