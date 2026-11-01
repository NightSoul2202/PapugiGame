// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpecHandle.h"
#include "CoopPlayerCharacter.generated.h"

class UCameraComponent;
class UInputComponent;
class UInputAction;
class UInputMappingContext;

UCLASS()
class PAPUGIGAME_API ACoopPlayerCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACoopPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;
	void StopSliding();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void StartCrouching();
	void StopCrounching();

	void Move(const FInputActionValue& InputValue);
	void Look(const FInputActionValue& InputValue);

	void Sprint();
	void StopSprinting();

	void Slide();

	UFUNCTION()
	virtual void OnDeath();

	FGameplayAbilitySpecHandle CurrentSprintSpeedHandle;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SlideAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<class UGameplayAbility> SprintAbilityClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TSubclassOf<class UGameplayAbility> SlideAbilityClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultInputContext;


private:
	void ActivateAbilityByClass(TSubclassOf<class UGameplayAbility> AbilityClass);
	void CancelAbilityByClass(TSubclassOf<class UGameplayAbility> AbilityClass);

	void DisableCollision();
	void EnableRagdoll();

};
