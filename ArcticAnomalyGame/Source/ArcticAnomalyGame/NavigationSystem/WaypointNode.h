// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "WaypointNode.generated.h"

UCLASS()
class ARCTICANOMALYGAME_API AWaypointNode : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties.
	AWaypointNode();
	
protected:
	// Called when the game starts or when spawned.
	virtual void BeginPlay() override;
	
public:
	// Called every frame.
	virtual void Tick(float DeltaTime) override;
	void DebugVisuals() const;
	
	// TODO: Disable this method entirely in game builds.
	virtual bool ShouldTickIfViewportsOnly() const override;

	// Gives a transform to the waypoint node.
	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY()
	UBoxComponent* EditorBoxCollider;
	
	// Array to store references to connected waypoints.
	UPROPERTY(EditAnywhere, Category = "Waypointing")
	TArray<AWaypointNode*> ConnectedWaypoints;
	
	// Boolean to determine if waypoint is a spawn point.
	UPROPERTY(EditAnywhere, Category = "Waypointing")
	bool IsSpawnPoint;

	// If true, the waypoint will be drawn during play.
	UPROPERTY(EditAnywhere, Category = "Waypointing")
	bool bDebugInPlay = false;

	UPROPERTY(EditAnywhere, Category = "Debugging")
	float DebugHeight = 300;
	
	UPROPERTY(EditAnywhere, Category = "Debugging")
	float DebugRadius = 150;
	
	UPROPERTY(EditAnywhere, Category = "Debugging")
	float DebugThickness = 5;
};
