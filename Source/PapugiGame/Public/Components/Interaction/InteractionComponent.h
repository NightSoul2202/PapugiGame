// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionFocusChanged, AActor*, NewFocus);

class AActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PAPUGIGAME_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionComponent();

	UFUNCTION(BlueprintCallable, Category = "Coop|Interaction")
	void TryInteract();

	UFUNCTION(Server, Reliable)
	void Server_Interact(AActor* InteractableTarget);

	UPROPERTY(BlueprintAssignable, Category = "Coop|Interaction")
	FOnInteractionFocusChanged OnFocusChanged;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Coop|Interaction")
	float InteractionDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Coop|Interaction")
	float TraceFrequency;

	UPROPERTY(Transient, EditDefaultsOnly, Category = "Coop|Interaction")
	TObjectPtr<AActor> CurrentFocus;

	FTimerHandle TimerHandle_Interaction;

	void UpdateFocus();
	
private:
	bool GetTracePoints(FVector& OutStart, FVector& OutEnd) const;
	TObjectPtr<AActor> PerformInteractionTrace() const;
	void ProcessNewFocus(AActor* NewFocus);
		
};
