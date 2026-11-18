// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Equipment/EquipmentComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Characters/HeroClassInfo.h"
#include "Equipment/Core/BaseEquipment.h"
#include "Components/SkeletalMeshComponent.h"

UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);
}

void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquipmentComponent, CurrentItem);
}

void UEquipmentComponent::OnRep_CurrentItem(ABaseEquipment* OldItem)
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

UAbilitySystemComponent* UEquipmentComponent::GetOwnerASC(AActor* Owner)
{
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner))
	{
		return ASI->GetAbilitySystemComponent();
	}
	return nullptr;
}

USkeletalMeshComponent* UEquipmentComponent::GetOwnerMesh(AActor* Owner)
{
	if (!Owner)
	{
		return nullptr;
	}
	
	return Owner->FindComponentByClass<USkeletalMeshComponent>();
}

void UEquipmentComponent::OnHeroInitialized(UHeroClassInfo* ClassInfo)
{
	if (!ClassInfo || !ClassInfo->StartingWeapon) return;

	FTransform SpawnTransform = GetOwner()->GetActorTransform();
	ABaseEquipment* SpawnedWeapon = GetWorld()->SpawnActorDeferred<ABaseEquipment>(
		ClassInfo->StartingWeapon,
		SpawnTransform, 
		GetOwner(), 
		Cast<APawn>(GetOwner()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (SpawnedWeapon)
	{
		SpawnedWeapon->FinishSpawning(SpawnTransform);

		EquipItem(SpawnedWeapon);
	}
}

bool UEquipmentComponent::Server_DropItem_Validate()
{
	return true;
}

void UEquipmentComponent::Server_DropItem_Implementation()
{
	UAbilitySystemComponent* ASC = GetOwnerASC(GetOwner());

	CurrentItem->UnequipItem(ASC);

	CurrentItem = nullptr;

	OnEquipmentChanged.Broadcast(nullptr);
}

bool UEquipmentComponent::Server_EquipItem_Validate(ABaseEquipment* ItemToEquip)
{
	return ItemToEquip != nullptr;
}

void UEquipmentComponent::Server_EquipItem_Implementation(ABaseEquipment* ItemToEquip)
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

void UEquipmentComponent::EquipItem(ABaseEquipment* ItemToEquip)
{
	if (ItemToEquip)
	{
		Server_EquipItem(ItemToEquip);
	}
}

void UEquipmentComponent::DropItem()
{
	if (CurrentItem)
	{
		Server_DropItem();
	}
}

ABaseEquipment* UEquipmentComponent::GetCurrentItem() const
{
	return CurrentItem;
}


