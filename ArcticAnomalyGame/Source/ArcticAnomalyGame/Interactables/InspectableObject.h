// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "InspectableObject.generated.h"

UCLASS()
class ARCTICANOMALYGAME_API AInspectableObject : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInspectableObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ObjectMesh;

	UPROPERTY(EditAnywhere)
	USceneComponent* HoldingComponent;

	UPROPERTY(EditAnywhere)
	float ObjectMoveSpeed;

	UFUNCTION()
	void RotateActor();

	UFUNCTION()
	void Pickup();

	bool holding;
	
	FRotator OriginalRotation;
	FVector OriginalLocation;
	FRotator ControlRotation;
	ACharacter* PlayerCharacter;
	UCameraComponent* PlayerCamera;
	FVector ForwardVector;
};
