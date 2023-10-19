// Copyright (c) 2023, Stinky Cheese, All rights reserved.


#include "SlidingDoor.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"


// Sets default values
ASlidingDoor::ASlidingDoor()
{
	DoorOpenPosition = DoorMesh->GetRelativeLocation()+FVector(0.0f, 100.0f, 0.0f);
	DoorClosePosition = DoorMesh->GetRelativeLocation();
	DoorCurrentPosition = FVector(0.0f, 0.0f, 0.0f);
	AddPosition = FVector(0.0f, 0.0f, 0.0f);
}

void ASlidingDoor::OpenDoor(float deltaTime)
{
	Super::OpenDoor(deltaTime);
	
	DoorCurrentPosition = DoorMesh->GetRelativeLocation();
	AddPosition = FMath::Lerp(DoorCurrentPosition, DoorTargetPosition, deltaTime * 2);
	
	if (DoorCurrentPosition.Equals(DoorTargetPosition, 0.25f))
	{
		Closing = false;
		Opening = false;
	}
	else if (Opening)
	{
		DoorMesh->SetRelativeLocation(AddPosition);
	}
}

void ASlidingDoor::CloseDoor(float deltaTime)
{
	Super::CloseDoor(deltaTime);
	
	DoorCurrentPosition = DoorMesh->GetRelativeLocation();
	AddPosition = FMath::Lerp(DoorCurrentPosition, DoorTargetPosition, deltaTime * 2);
	
	if (DoorCurrentPosition.Equals(DoorTargetPosition, 0.25f))
	{
		Closing = false;
		Opening = false;
	}
	else if (Closing)
	{
		DoorMesh->SetRelativeLocation(AddPosition);
	}
}

void ASlidingDoor::ToggleDoor(FVector ForwardVector)
{
	Super::ToggleDoor(ForwardVector);
		
	
	DoorTargetPosition = isClosed ? DoorClosePosition : DoorOpenPosition;
}