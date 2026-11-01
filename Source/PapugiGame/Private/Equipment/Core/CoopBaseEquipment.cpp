// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/Core/CoopBaseEquipment.h"
#include "Components/Equipment/CoopEquipmentComponent.h"
#include "AbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameplayEffect.h"
#include "GameplayAbilitySpec.h"
#include "Net/UnrealNetwork.h"

ACoopBaseEquipment::ACoopBaseEquipment()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SetReplicateMovement(true);

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;
	CollisionComponent->SetBoxExtent(FVector(30.f, 10.f, 10.f));
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionComponent->SetSimulatePhysics(true);

	EquipmentMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EquipmentMesh"));
	EquipmentMesh->SetupAttachment(RootComponent);
	EquipmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AttachmentSocketName = FName("RightHandSocket");
}

void ACoopBaseEquipment::Interact_Implementation(AActor* InstigatorActor)
{
	if (!InstigatorActor)
	{
		return;
	}

	UCoopEquipmentComponent* EquipComp = InstigatorActor->FindComponentByClass<UCoopEquipmentComponent>();

	if (EquipComp)
	{
		EquipComp->EquipItem(this);
	}
}

USkeletalMeshComponent* ACoopBaseEquipment::GetEquipmentMesh() const
{
	return EquipmentMesh;
}

void ACoopBaseEquipment::EquipItem(UAbilitySystemComponent* TargetASC, USkeletalMeshComponent* TargetMesh)
{
	if (!TargetASC || !TargetMesh) return;

	if (HasAuthority())
	{
		AActor* AvatarActor = TargetASC->GetAvatarActor();
		SetOwner(AvatarActor);
		SetInstigator(Cast<APawn>(AvatarActor));
	}

	CollisionComponent->SetSimulatePhysics(false);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AttachToComponent(TargetMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachmentSocketName);

	if (HasAuthority())
	{
		AddGrantedAbilities(TargetASC);
		AddGrantedEffects(TargetASC);
	}
}

void ACoopBaseEquipment::UnequipItem(UAbilitySystemComponent* TargetASC)
{
	if (!TargetASC) return;

	if (HasAuthority())
	{
		RemoveGrantedAbilities(TargetASC);
		RemoveGrantedEffects(TargetASC);

		SetOwner(nullptr);
		SetInstigator(nullptr);
	}

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetSimulatePhysics(true);

	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
}

void ACoopBaseEquipment::AddGrantedAbilities(UAbilitySystemComponent* TargetASC)
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

void ACoopBaseEquipment::AddGrantedEffects(UAbilitySystemComponent* TargetASC)
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

void ACoopBaseEquipment::RemoveGrantedAbilities(UAbilitySystemComponent* TargetASC)
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

void ACoopBaseEquipment::RemoveGrantedEffects(UAbilitySystemComponent* TargetASC)
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