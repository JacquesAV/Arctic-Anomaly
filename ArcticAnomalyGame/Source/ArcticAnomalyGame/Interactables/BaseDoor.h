// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "BaseDoor.generated.h"

UCLASS()
class ARCTICANOMALYGAME_API ABaseDoor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void CloseDoor(float deltaTime);

	UFUNCTION()
	virtual void OpenDoor(float deltaTime);

	UFUNCTION()
	virtual void ToggleDoor(FVector ForwardVector);
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(VisibleAnywhere, Category = "Collider")
	UBoxComponent* ColliderComponent;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(VisibleAnywhere, Category = "Box Comps")
	UBoxComponent* BoxComp;

	bool Opening;
	bool Closing;
	bool isClosed;
};
