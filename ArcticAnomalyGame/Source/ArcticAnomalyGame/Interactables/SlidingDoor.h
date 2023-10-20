// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseDoor.h"
#include "GameFramework/Actor.h"
#include "SlidingDoor.generated.h"

UCLASS()
class ARCTICANOMALYGAME_API ASlidingDoor : public ABaseDoor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASlidingDoor();

public:
	// Called every frame	
	virtual void CloseDoor(float deltaTime) override;

	virtual void OpenDoor(float deltaTime) override;

	virtual void ToggleDoor(FVector ForwardVector) override;

	UPROPERTY(EditAnywhere)
	FVector DoorClosePosition;
	UPROPERTY(EditAnywhere)
	FVector DoorOpenPosition;
	FVector DoorCurrentPosition;
	FVector DoorTargetPosition;
	FVector AddPosition;
	UPROPERTY(EditAnywhere)
	float SlideSpeed;
};
