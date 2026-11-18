// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Coop_PlayerCharacter.h"
#include "Components/Movement/Coop_CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"
#include "Core/Coop_PlayerState.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Input/InputConfig.h"
#include "Components/Interaction/InteractionComponent.h"
#include "Components/Equipment/EquipmentComponent.h"
#include "Components/Health/HealthComponent.h"
#include <Components/ClassSystem/HeroClassComponent.h>

ACoop_PlayerCharacter::ACoop_PlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCoop_CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->bUsePawnControlRotation = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
	HeroClassComponent = CreateDefaultSubobject<UHeroClassComponent>(TEXT("HeroClassComponent"));

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void ACoop_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UCoop_CharacterMovementComponent* CharMovement = Cast<UCoop_CharacterMovementComponent>(GetCharacterMovement()))
	{
		CharMovement->OnSlideExitedDelegate.AddUObject(this, &ACoop_PlayerCharacter::StopSliding);
	}

	if (HealthComponent)
	{
		HealthComponent->OnCharacterDeathDelegate.AddDynamic(this, &ACoop_PlayerCharacter::OnDeath);
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(DefaultInputContext, 0);
		}
	}
}

void ACoop_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACoop_PlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACoop_PlayerCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ACoop_PlayerCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACoop_PlayerCharacter::StopSprinting);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ACoop_PlayerCharacter::StartCrouching);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ACoop_PlayerCharacter::StopCrounching);
		EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Started, this, &ACoop_PlayerCharacter::Slide);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ACoop_PlayerCharacter::Interact);
		EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Started, this, &ACoop_PlayerCharacter::Drop);

		if (InputConfig)
		{
			for (const FCoop_InputAction& Action : InputConfig->InputActions)
			{
				if (Action.InputAction && Action.InputTag.IsValid())
				{
					EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Started, this, &ACoop_PlayerCharacter::Input_AbilityInputTagPressed, Action.InputTag);
					EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Completed, this, &ACoop_PlayerCharacter::Input_AbilityInputTagReleased, Action.InputTag);
					EnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Canceled, this, &ACoop_PlayerCharacter::Input_AbilityInputTagReleased, Action.InputTag);
				}
			}
		}
	}
}

void ACoop_PlayerCharacter::StartCrouching()
{
	Super::Crouch(true);
}

void ACoop_PlayerCharacter::StopCrounching()
{
	Super::UnCrouch(true);
}

void ACoop_PlayerCharacter::Move(const FInputActionValue& InputValue)
{
	const FVector2D MoveVector = InputValue.Get<FVector2D>();

	if (Controller)
	{
		const FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, MoveVector.Y);
		
		const FVector Right = GetActorRightVector();
		AddMovementInput(Right, MoveVector.X);
	}
}

void ACoop_PlayerCharacter::Look(const FInputActionValue& InputValue)
{
	const FVector2D LookVector = InputValue.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void ACoop_PlayerCharacter::Sprint()
{
	ActivateAbilityByClass(SprintAbilityClass);
}

void ACoop_PlayerCharacter::StopSprinting()
{
	CancelAbilityByClass(SprintAbilityClass);
}

void ACoop_PlayerCharacter::Slide()
{
	ActivateAbilityByClass(SlideAbilityClass);
}

void ACoop_PlayerCharacter::OnDeath()
{
	if (HasAuthority())
	{
		Multicast_OnDeath();
	}
}

void ACoop_PlayerCharacter::Multicast_OnDeath_Implementation()
{
	if (IsLocallyControlled())
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			DisableInput(PC);
		}
	}

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Death")));
	}

	if (HasAuthority())
	{
		Drop();
	}

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->DisableMovement();
		MoveComp->StopMovementImmediately();
	}
	DisableCollision();

	EnableRagdoll();
}

void ACoop_PlayerCharacter::StopSliding()
{
	CancelAbilityByClass(SlideAbilityClass);
}

void ACoop_PlayerCharacter::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
		{
			if (Spec.Ability && (Spec.DynamicAbilityTags.HasTagExact(InputTag) || Spec.Ability->AbilityTags.HasTagExact(InputTag)))
			{
				ASC->AbilitySpecInputPressed(Spec);

				if (!Spec.IsActive())
				{
					ASC->TryActivateAbility(Spec.Handle);
				}
			}
		}
	}
}

void ACoop_PlayerCharacter::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
		{
			if (Spec.Ability && (Spec.DynamicAbilityTags.HasTagExact(InputTag) || Spec.Ability->AbilityTags.HasTagExact(InputTag)))
			{
				ASC->AbilitySpecInputReleased(Spec);
			}
		}
	}
}

void ACoop_PlayerCharacter::Interact()
{
	if (UInteractionComponent* InteractComp = FindComponentByClass<UInteractionComponent>())
	{
		InteractComp->TryInteract();
	}
}

void ACoop_PlayerCharacter::Drop()
{
	if (UEquipmentComponent* EquipComp = FindComponentByClass<UEquipmentComponent>())
	{
		EquipComp->DropItem();
	}
}

void ACoop_PlayerCharacter::ActivateAbilityByClass(TSubclassOf<class UGameplayAbility> AbilityClass)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (AbilityClass)
		{
			ASC->TryActivateAbilityByClass(AbilityClass);
		}
	}
}

void ACoop_PlayerCharacter::CancelAbilityByClass(TSubclassOf<class UGameplayAbility> AbilityClass)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (AbilityClass)
		{
			ASC->CancelAbility(AbilityClass.GetDefaultObject());
		}
	}
}

void ACoop_PlayerCharacter::DisableCollision()
{
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	}
}

void ACoop_PlayerCharacter::EnableRagdoll()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComp->SetSimulatePhysics(true);
	}
}

UAbilitySystemComponent* ACoop_PlayerCharacter::GetAbilitySystemComponent() const
{
	if (ACoop_PlayerState* PS = GetPlayerState<ACoop_PlayerState>())
	{
		return PS->GetAbilitySystemComponent();
	}

	return nullptr;
}

void ACoop_PlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ACoop_PlayerState* PS = GetPlayerState<ACoop_PlayerState>())
	{
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		if (HealthComponent)
		{
			HealthComponent->InitializeHealthComponent(PS->GetAbilitySystemComponent());
		}

		if (HeroClassComponent)
		{
			HeroClassComponent->InitializeClass();
		}

		if (HasAuthority())
		{
			if (SprintAbilityClass && !CurrentSprintSpeedHandle.IsValid())
			{
				CurrentSprintSpeedHandle = PS->GetAbilitySystemComponent()->GiveAbility
				(
					FGameplayAbilitySpec(SprintAbilityClass, 1, INDEX_NONE, this)
				);
			}

			if (SlideAbilityClass)
			{
				PS->GetAbilitySystemComponent()->GiveAbility
				(
					FGameplayAbilitySpec(SlideAbilityClass, 1, INDEX_NONE, this)
				);
			}
		}

	}
}

void ACoop_PlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (ACoop_PlayerState* PS = GetPlayerState<ACoop_PlayerState>())
	{
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		if (HealthComponent)
		{
			HealthComponent->InitializeHealthComponent(PS->GetAbilitySystemComponent());
		}
	}
}

