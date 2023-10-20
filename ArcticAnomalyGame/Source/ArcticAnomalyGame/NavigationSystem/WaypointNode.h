// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#pragma once

#include "CoreMinimal.h"
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
	virtual bool ShouldTickIfViewportsOnly() const override;

	// Gives a transform to the waypoint node.
	UPROPERTY()
	USceneComponent* Root;
	
	// Array to store references to connected waypoints.
	UPROPERTY(EditAnywhere, Category = "Waypointing")
	TArray<AWaypointNode*> ConnectedWaypoints;
	
	// Boolean to determine if waypoint is a spawn point.
	UPROPERTY(EditAnywhere, Category = "Waypointing")
	bool IsSpawnPoint;
};
