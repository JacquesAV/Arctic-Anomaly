// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#include "SwingDoor.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"

// Sets default values
ASwingDoor::ASwingDoor()
{
	DotProduct = 0.0f;
	MaxDegree = 0.0f;
	PosNeg = 0.0f;
	DoorCurrentRotation = 0.0f;
}

void ASwingDoor::OpenDoor(float deltaTime)
{
	Super::OpenDoor(deltaTime);

	DoorCurrentRotation = DoorMesh->GetRelativeRotation().Yaw;

	AddRotation = PosNeg * deltaTime * 80;

	if (FMath::IsNearlyEqual(DoorCurrentRotation, MaxDegree, 1.5f))
	{
		Closing = false;
		Opening = false;
	}
	else if (Opening)
	{
		FRotator newRotation = FRotator(0.0f, AddRotation, 0.0f);
		DoorMesh->AddRelativeRotation(FQuat(newRotation), false, 0, ETeleportType::None);
	}
}

void ASwingDoor::CloseDoor(float deltaTime)
{
	Super::CloseDoor(deltaTime);

	DoorCurrentRotation = DoorMesh->GetRelativeRotation().Yaw;

	if (DoorCurrentRotation > 0)
	{
		AddRotation = -deltaTime * 80;
	}
	else
	{
		AddRotation = deltaTime * 80;
	}

	if (FMath::IsNearlyEqual(DoorCurrentRotation, 0.0f, 1.5f))
	{
		Closing = false;
		Opening = false;
	}
	else if (Closing)
	{
		FRotator newRotation = FRotator(0.0f, AddRotation, 0.0f);
		DoorMesh->AddRelativeRotation(FQuat(newRotation), false, 0, ETeleportType::None);
	}
}

void ASwingDoor::ToggleDoor(FVector ForwardVector)
{
	Super::ToggleDoor(ForwardVector);

	DotProduct = FVector::DotProduct(BoxComp->GetForwardVector(), ForwardVector);

	PosNeg = FMath::Sign(DotProduct);

	MaxDegree = PosNeg * 90;
}
