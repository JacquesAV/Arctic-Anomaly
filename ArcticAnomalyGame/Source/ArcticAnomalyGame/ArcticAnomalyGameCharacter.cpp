// Copyright Epic Games, Inc. All Rights Reserved.

#include "ArcticAnomalyGameCharacter.h"
#include "ArcticAnomalyGameProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "InventorySystem/Items/Item.h"
#include "Engine/TriggerCapsule.h"
#include "Interactables/InspectableObject.h"
#include "InventorySystem/ItemPickup.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AArcticAnomalyGameCharacter

AArcticAnomalyGameCharacter::AArcticAnomalyGameCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	//Setup Inventory
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	// Create a trigger capsule component
	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("TriggerCapsule"));
	TriggerCapsule->InitCapsuleSize(55.f, 96.0f);
	TriggerCapsule->SetCollisionProfileName(TEXT("Trigger"));
	TriggerCapsule->SetupAttachment(RootComponent);

	TriggerCapsule->OnComponentBeginOverlap.AddDynamic(this, &AArcticAnomalyGameCharacter::OnOverlapBegin);
	TriggerCapsule->OnComponentEndOverlap.AddDynamic(this, &AArcticAnomalyGameCharacter::OnOverlapEnd);

	CurrentDoor = NULL;

	//Setup Inspection Component
	HoldingComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HoldingComponent"));
	FVector HoldingComponentLocation = HoldingComponent->GetRelativeLocation();
	HoldingComponent->SetRelativeLocation(FVector(HoldingComponentLocation.X, HoldingComponentLocation.Y,
	                                              HoldingComponentLocation.Z));
	HoldingComponent->SetupAttachment(RootComponent);

	CurrentInspectable = nullptr;
	CanMove = true;
	Inspecting = false;
}

void AArcticAnomalyGameCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	PitchMax = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax;
	PitchMin = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMin;
}

void AArcticAnomalyGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Start = FirstPersonCameraComponent->GetComponentLocation();
	ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	End = ((ForwardVector * 200.0f) + Start);

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

	if (!HoldingObject)
	{
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, DefaultComponentQueryParams,
		                                         DefaultResponseParams))
		{
			if (Hit.GetActor()->GetClass()->IsChildOf(AInspectableObject::StaticClass()))
			{
				CurrentInspectable = Cast<AInspectableObject>(Hit.GetActor());
			}
		}
		else
		{
			CurrentInspectable = nullptr;
		}

		if (Inspecting)
		{
			if (HoldingObject)
			{
				FirstPersonCameraComponent->SetFieldOfView(
					FMath::Lerp(FirstPersonCameraComponent->FieldOfView, 90.0f, 0.1f));

				HoldingComponent->SetRelativeLocation(FVector(0.0f, 50.0f, 50.0f));
				GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax = 179.90000002f;
				GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMin = -179.90000002f;
				CurrentInspectable->RotateActor();
			}
			else
			{
				FirstPersonCameraComponent->SetFieldOfView(
					FMath::Lerp(FirstPersonCameraComponent->FieldOfView, 45.0f, 0.1f));
			}
		}
		else
		{
			FirstPersonCameraComponent->SetFieldOfView(
				FMath::Lerp(FirstPersonCameraComponent->FieldOfView, 90.0f, 0.1f));

			if (HoldingObject)
			{
				HoldingComponent->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void AArcticAnomalyGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this,
		                                   &AArcticAnomalyGameCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this,
		                                   &AArcticAnomalyGameCharacter::Look);

		//Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this,
		                                   &AArcticAnomalyGameCharacter::Interact);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}


void AArcticAnomalyGameCharacter::Move(const FInputActionValue& Value)
{
	if (!CanMove)
		return;
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AArcticAnomalyGameCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AArcticAnomalyGameCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AArcticAnomalyGameCharacter::GetHasRifle()
{
	return bHasRifle;
}

#pragma region Interact

void AArcticAnomalyGameCharacter::Interact()
{
	DoorInteraction();
	ItemInteraction();
	InspectInteraction();
}

void AArcticAnomalyGameCharacter::DoorInteraction()
{
	if (CurrentDoor != nullptr)
	{
		ForwardVector = FirstPersonCameraComponent->GetForwardVector();
		CurrentDoor->ToggleDoor(ForwardVector);
	}
}

void AArcticAnomalyGameCharacter::ItemInteraction()
{
	if (CurrentItemPickup != nullptr && CurrentItemPickup->Item)
	{
		Inventory->AddItem(CurrentItemPickup->Item);
		CurrentItemPickup->Destroy();
	}
}

void AArcticAnomalyGameCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                 const FHitResult& SweepResult)
{
	//make sure it doesnt overlap with itself
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr)
	{
		if (OtherActor->GetClass()->IsChildOf(ABaseDoor::StaticClass()))
			CurrentDoor = Cast<ABaseDoor>(OtherActor);

		if (OtherActor->GetClass()->IsChildOf(AItemPickup::StaticClass()))
		{
			AItemPickup* ItemPickup = Cast<AItemPickup>(OtherActor);
			CurrentItemPickup = ItemPickup;
		}
	}
}

void AArcticAnomalyGameCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr && OtherActor != this && OtherComp != nullptr)
	{
		CurrentDoor = NULL;
		CurrentItemPickup = NULL;
	}
}

#pragma endregion

#pragma region Inspect

//meant for on action originally
/*if (CurrentInspectable && !Inspecting)
{
	ToggleObjectInspection();
}*/

void AArcticAnomalyGameCharacter::InspectInteraction()
{
	if (HoldingObject)
	{
		LastRotation = GetControlRotation();
		ToggleMovement();
	}
	else
	{
		Inspecting = true;
	}
}

void AArcticAnomalyGameCharacter::InspectReleased()
{
	if (Inspecting && HoldingObject)
	{
		GetController()->SetControlRotation(LastRotation);
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax = PitchMax;
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMin = PitchMin;
		ToggleMovement();
	}
	else
	{
		Inspecting = false;
	}
}

void AArcticAnomalyGameCharacter::ToggleMovement()
{
	CanMove = !CanMove;
	Inspecting = !Inspecting;
	FirstPersonCameraComponent->bUsePawnControlRotation = !FirstPersonCameraComponent->bUsePawnControlRotation;
	bUseControllerRotationYaw = !bUseControllerRotationYaw;
}

void AArcticAnomalyGameCharacter::ToggleObjectInspection()
{
	if (CurrentInspectable)
	{
		HoldingObject = !HoldingObject;
		CurrentInspectable->Pickup();

		if(!HoldingObject)
			CurrentInspectable = nullptr;
	}
}


#pragma endregion
