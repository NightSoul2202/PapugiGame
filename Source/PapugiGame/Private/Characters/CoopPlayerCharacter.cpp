// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CoopPlayerCharacter.h"
#include "Components/Movement/CoopCharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "EnhancedInputSubsystems.h"
#include <Core/CoopPlayerState.h>
#include "Components/CapsuleComponent.h"
#include "GameFramework/GameModeBase.h"

ACoopPlayerCharacter::ACoopPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCoopCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->bUsePawnControlRotation = true;

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void ACoopPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UCoopCharacterMovementComponent* CharMovement = Cast<UCoopCharacterMovementComponent>(GetCharacterMovement()))
	{
		CharMovement->OnSlideExitedDelegate.AddUObject(this, &ACoopPlayerCharacter::StopSliding);
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(DefaultInputContext, 0);
		}
	}
}

void ACoopPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACoopPlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACoopPlayerCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ACoopPlayerCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACoopPlayerCharacter::StopSprinting);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ACoopPlayerCharacter::StartCrouching);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ACoopPlayerCharacter::StopCrounching);
		EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Started, this, &ACoopPlayerCharacter::Slide);
	}
}

void ACoopPlayerCharacter::StartCrouching()
{
	Super::Crouch(true);
}

void ACoopPlayerCharacter::StopCrounching()
{
	Super::UnCrouch(true);
}

void ACoopPlayerCharacter::Move(const FInputActionValue& InputValue)
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

void ACoopPlayerCharacter::Look(const FInputActionValue& InputValue)
{
	const FVector2D LookVector = InputValue.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void ACoopPlayerCharacter::Sprint()
{
	ActivateAbilityByClass(SprintAbilityClass);
}

void ACoopPlayerCharacter::StopSprinting()
{
	CancelAbilityByClass(SprintAbilityClass);
}

void ACoopPlayerCharacter::Slide()
{
	ActivateAbilityByClass(SlideAbilityClass);
}

void ACoopPlayerCharacter::OnDeath()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	DisableCollision();

	EnableRagdoll();

	if (HasAuthority())
	{
		/*AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();*/

		DetachFromControllerPendingDestroy();
	}
}

void ACoopPlayerCharacter::StopSliding()
{
	CancelAbilityByClass(SlideAbilityClass);
}

void ACoopPlayerCharacter::ActivateAbilityByClass(TSubclassOf<class UGameplayAbility> AbilityClass)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (AbilityClass)
		{
			ASC->TryActivateAbilityByClass(AbilityClass);
		}
	}
}

void ACoopPlayerCharacter::CancelAbilityByClass(TSubclassOf<class UGameplayAbility> AbilityClass)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (AbilityClass)
		{
			ASC->CancelAbility(AbilityClass.GetDefaultObject());
		}
	}
}

void ACoopPlayerCharacter::DisableCollision()
{
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	}
}

void ACoopPlayerCharacter::EnableRagdoll()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComp->SetSimulatePhysics(true);
	}
}

UAbilitySystemComponent* ACoopPlayerCharacter::GetAbilitySystemComponent() const
{
	if (ACoopPlayerState* PS = GetPlayerState<ACoopPlayerState>())
	{
		return PS->GetAbilitySystemComponent();
	}

	return nullptr;
}

void ACoopPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (ACoopPlayerState* PS = GetPlayerState<ACoopPlayerState>())
	{
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		PS->OnOutHealthDelegate.AddUObject(this, &ACoopPlayerCharacter::OnDeath);

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

void ACoopPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (ACoopPlayerState* PS = GetPlayerState<ACoopPlayerState>())
	{
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		PS->OnOutHealthDelegate.AddUObject(this, &ACoopPlayerCharacter::OnDeath);
	}
}

