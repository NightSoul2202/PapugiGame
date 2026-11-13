// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Notifies/ANS_SendGameplayEvent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

void UANS_SendGameplayEvent::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
	{
		FGameplayEventData EventData;
		EventData.EventTag = EventTag;
		EventData.Instigator = MeshComp->GetOwner();
		EventData.Target = MeshComp->GetOwner();

		ASC->HandleGameplayEvent(EventTag, &EventData);
	}
}