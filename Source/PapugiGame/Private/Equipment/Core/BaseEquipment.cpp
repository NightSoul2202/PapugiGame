// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/Core/BaseEquipment.h"
#include "Components/Equipment/EquipmentComponent.h"
#include "AbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameplayEffect.h"
#include "GameplayAbilitySpec.h"
#include "Net/UnrealNetwork.h"

ABaseEquipment::ABaseEquipment()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	EquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EquipmentMesh"));
	RootComponent = EquipmentMesh;

	EquipmentMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	EquipmentMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	EquipmentMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	EquipmentMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	EquipmentMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	EquipmentMesh->SetSimulatePhysics(true);

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(RootComponent);
	CollisionComponent->SetBoxExtent(FVector(30.f, 10.f, 10.f));

	CollisionComponent->SetSimulatePhysics(false);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	AttachmentSocketName = FName("RightHandSocket");
}

void ABaseEquipment::Interact_Implementation(AActor* InstigatorActor)
{
	if (!InstigatorActor)
	{
		return;
	}

	UEquipmentComponent* EquipComp = InstigatorActor->FindComponentByClass<UEquipmentComponent>();

	if (EquipComp)
	{
		EquipComp->EquipItem(this);
	}
}

USkeletalMeshComponent* ABaseEquipment::GetEquipmentMesh() const
{
	return EquipmentMesh;
}

void ABaseEquipment::EquipItem(UAbilitySystemComponent* TargetASC, USkeletalMeshComponent* TargetMesh)
{
	if (!TargetASC || !TargetMesh) return;

	if (HasAuthority())
	{
		AActor* AvatarActor = TargetASC->GetAvatarActor();
		SetOwner(AvatarActor);
		SetInstigator(Cast<APawn>(AvatarActor));
	}

	EquipmentMesh->SetSimulatePhysics(false);
	EquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AttachToComponent(TargetMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachmentSocketName);

	if (HasAuthority())
	{
		AddGrantedAbilities(TargetASC);
		AddGrantedEffects(TargetASC);
	}
}

void ABaseEquipment::UnequipItem(UAbilitySystemComponent* TargetASC)
{
	if (!TargetASC) return;

	if (HasAuthority())
	{
		RemoveGrantedAbilities(TargetASC);
		RemoveGrantedEffects(TargetASC);

		SetOwner(nullptr);
		SetInstigator(nullptr);
	}

	EquipmentMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	EquipmentMesh->SetSimulatePhysics(true);

	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
}

void ABaseEquipment::AddGrantedAbilities(UAbilitySystemComponent* TargetASC)
{
	for (TSubclassOf<UGameplayAbility> AbilityClass : GrantedAbilities)
	{
		if (AbilityClass)
		{
			FGameplayAbilitySpec AbilitySpec(AbilityClass, 1, INDEX_NONE, this);

			FGameplayAbilitySpecHandle Handle = TargetASC->GiveAbility(AbilitySpec);

			GrantedAbilityHandles.Add(Handle);
		}
	}
}

void ABaseEquipment::AddGrantedEffects(UAbilitySystemComponent* TargetASC)
{
	for (TSubclassOf<UGameplayEffect> EffectClass : GrantedEffects)
	{
		if (EffectClass)
		{
			FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
			ContextHandle.AddInstigator(this, this);

			FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(EffectClass, 1.0f, ContextHandle);

			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

				GrantedEffectHandles.Add(ActiveHandle);
			}
		}
	}
}

void ABaseEquipment::RemoveGrantedAbilities(UAbilitySystemComponent* TargetASC)
{
	for (const FGameplayAbilitySpecHandle& AbilityHandle : GrantedAbilityHandles)
	{
		if (AbilityHandle.IsValid())
		{
			TargetASC->ClearAbility(AbilityHandle);
		}
	}

	GrantedAbilityHandles.Empty();
}

void ABaseEquipment::RemoveGrantedEffects(UAbilitySystemComponent* TargetASC)
{
	for (const FActiveGameplayEffectHandle& EffectHandle : GrantedEffectHandles)
	{
		if (EffectHandle.IsValid())
		{
			TargetASC->RemoveActiveGameplayEffect(EffectHandle);
		}
	}

	GrantedEffectHandles.Empty();
}