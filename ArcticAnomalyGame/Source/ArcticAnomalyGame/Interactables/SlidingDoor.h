// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlidingDoor.generated.h"

UCLASS()
class ARCTICANOMALYGAME_API ASlidingDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlidingDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void CloseDoor(float deltaTime);

	UFUNCTION()
	void OpenDoor(float deltaTime);

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(VisibleAnywhere, Category = "Box Comps")
	class UBoxComponent* BoxComp;

	UFUNCTION()
	void ToggleDoor(FVector ForwardVector);

	bool Opening;
	bool Closing;
	bool isClosed;

	float DotProduct;
	float MaxDegree;
	float AddRotation;
	float PosNeg;
	float DoorCurrentRotation;
};
