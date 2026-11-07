// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/Components/FirearmComponent.h"
#include "Animation/AnimMontage.h"
#include "Net/UnrealNetwork.h"

UFirearmComponent::UFirearmComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	SupportedFireModes.Add(EFireMode::Single);
}


void UFirearmComponent::Server_SwitchFireMode_Implementation()
{
	if (SupportedFireModes.Num() <= 1)
	{
		return;
	}

	int32 CurrentIndex = SupportedFireModes.Find(CurrentFireMode);
	int32 NextIndex = (CurrentIndex + 1) % SupportedFireModes.Num();

	CurrentFireMode = SupportedFireModes[NextIndex];
}

void UFirearmComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority() && SupportedFireModes.Num() > 0)
	{
		CurrentFireMode = SupportedFireModes[0];
	}
}

void UFirearmComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFirearmComponent, CurrentFireMode);
}


