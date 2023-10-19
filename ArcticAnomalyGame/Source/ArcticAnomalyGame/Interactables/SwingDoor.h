// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseDoor.h"
#include "GameFramework/Actor.h"
#include "SwingDoor.generated.h"

UCLASS()
class ARCTICANOMALYGAME_API ASwingDoor : public ABaseDoor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASwingDoor();

public:
	// Called every frame	
	virtual void CloseDoor(float deltaTime) override;

	virtual void OpenDoor(float deltaTime) override;

	virtual void ToggleDoor(FVector ForwardVector) override;

	float DotProduct;
	float MaxDegree;
	float AddRotation;
	float PosNeg;
	float DoorCurrentRotation;
};
