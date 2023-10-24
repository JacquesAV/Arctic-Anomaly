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
#include "WinZone.h"
#include "Engine/LocalPlayer.h"
#include "InventorySystem/Items/Item.h"
#include "Engine/TriggerCapsule.h"
#include "Interactables/InspectableObject.h"
#include "InventorySystem/ItemPickup.h"
#include "Kismet/GameplayStatics.h"

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

	//initialise the data manager
	DataManager = CreateDefaultSubobject<UCustomDataManager>(TEXT("DataManager"));

	//Setup Inspection Component
	HoldingComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HoldingComponent"));
	HoldingComponent->SetRelativeLocation(InspectableObjectOffset);
	HoldingComponent->SetupAttachment(FirstPersonCameraComponent);

	CurrentInspectable = nullptr;
	CanMove = true;
	Zooming = false;
	Rotating = false;
	InspectingObject = false;
}

void AArcticAnomalyGameCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//iterate through the required items and add them to the DataManagerArray and set the value to false
	for (int i = 0; i < RequiredItems.Num(); i++)
	{
		FItemBoolPair NewPair(RequiredItems[i], false);
		DataManager->AddKeyValuePair(NewPair);
	}

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	//iterate through the required items and log the value of each dataManager keyvalue pair
	for (int i = 0; i < RequiredItems.Num(); i++)
	{
		bool Value;
		if (RequiredItems[i] && !RequiredItems[i]->ItemDisplayName.IsEmpty() && DataManager->GetValueForKey(
			RequiredItems[i], Value))
		{
			UE_LOG(LogTemp, Warning, TEXT("Key: %s Value: %s"), *RequiredItems[i]->ItemDisplayName.ToString(),
			       Value ? TEXT("True") : TEXT("False"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Required items failed to initialize properly"))
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

	//If the player is not inspecting an object, check for one in front of the player
	if (!InspectingObject)
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
	}

	//If the player is trying to rotate the object whilst they are inspecting
	if (InspectingObject && Rotating)
	{
		HoldingComponent->SetRelativeLocation(InspectableObjectOffset);
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax = 179.90000002f;
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMin = -179.90000002f;
		CurrentInspectable->RotateActor();
	}
	else
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax = PitchMax;
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMin = PitchMin;
	}

	//If the player is trying to zoom and they are not inspecting
	if (Zooming)
	{
		FirstPersonCameraComponent->SetFieldOfView(
			FMath::Lerp(FirstPersonCameraComponent->FieldOfView, 45.0f, 0.1f));
	}
	//If the is no object to inspect and playing is not zooming, set the field of view back to normal
	else
	{
		FirstPersonCameraComponent->SetFieldOfView(
			FMath::Lerp(FirstPersonCameraComponent->FieldOfView, 90.0f, 0.1f));
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

		//Inspecting
		EnhancedInputComponent->BindAction(InspectAction, ETriggerEvent::Triggered, this,
		                                   &AArcticAnomalyGameCharacter::InspectPressed);
		EnhancedInputComponent->BindAction(InspectAction, ETriggerEvent::Completed, this,
		                                   &AArcticAnomalyGameCharacter::InspectReleased);
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
	InspectObjectInteraction();
}

bool AArcticAnomalyGameCharacter::HasFoundItem(UItem* Item)
{
	for (int i = 0; i < RequiredItems.Num(); i++)
	{
		bool Value;
		if (DataManager->GetValueForKey(RequiredItems[i], Value))
		{
			if (RequiredItems[i] == Item)
				return Value;
		}
	}
	return false;
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
		if (Inventory->AddItem(CurrentItemPickup->Item))
		{
			//find the item in the data manager and set the value to true
			DataManager->SetValueForKey(CurrentItemPickup->Item, true);

			//log if all items are true
			bool AllTrue;
			DataManager->AllValuesTrue(AllTrue);
			UE_LOG(LogTemp, Warning, TEXT("AllTrue: %s"), AllTrue ? TEXT("True") : TEXT("False"));
			
			HasAllRequiredItems = AllTrue;
		}
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
		
		//if the player has all the required items and the object they are overlapping with is the win zone
		if (HasAllRequiredItems && OtherActor->GetClass()->IsChildOf(AWinZone::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Player has won!"));
			//load the victory level
			UGameplayStatics::OpenLevel(GetWorld(), "GameWin");
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

void AArcticAnomalyGameCharacter::InspectObjectInteraction()
{
	if (CurrentInspectable)
	{
		ToggleObjectInspection();
		ToggleMovement();
	}
}

void AArcticAnomalyGameCharacter::InspectPressed()
{
	Zooming = true;
}

void AArcticAnomalyGameCharacter::InspectReleased()
{
	Zooming = false;
}

//Return the player to normal movement
void AArcticAnomalyGameCharacter::ToggleMovement()
{
	CanMove = !CanMove;
	Rotating = !Rotating;
	FirstPersonCameraComponent->bUsePawnControlRotation = !FirstPersonCameraComponent->bUsePawnControlRotation;
	bUseControllerRotationYaw = !bUseControllerRotationYaw;
	//log the last rotation
	if (CanMove)
		GetController()->SetControlRotation(LastRotation);
	else
		LastRotation = GetControlRotation();
}

void AArcticAnomalyGameCharacter::ToggleObjectInspection()
{
	//If the player is looking at an inspectable object
	if (CurrentInspectable)
	{
		//Start inspecting the object that the player is viewing
		InspectingObject = !InspectingObject;
		CurrentInspectable->Pickup();

		//if the player is not inspecting anything, set to null
		if (!InspectingObject)
			CurrentInspectable = nullptr;
	}
}


#pragma endregion
