// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaypointNode.h"
#include "PathHolderManager.generated.h"

//TODO: Apply editor-only tags to some of these properties to prevent them from being built into the game.
UCLASS()
class ARCTICANOMALYGAME_API APathHolderManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APathHolderManager();
	
	// Gives a transform to the waypoint node.
	UPROPERTY()
	USceneComponent* Root;
	
	// Connects waypoints bidirectionally in-editor for ease of linking.
	UFUNCTION(CallInEditor, Category = "Waypointing")
	void ConnectWaypointsBidirectionally() const;
	
	// Fetches all waypoints nested under this actor and stores them in the Waypoints array.
	UFUNCTION(CallInEditor, Category = "Waypointing")
	void UpdatePathWaypoints();
	
	// String name of the path grouping
	UPROPERTY(EditAnywhere, Category = "Waypointing")
	FString Name;
	
	// Array of all waypoints associated with this path.
	UPROPERTY(EditAnywhere, Category = "Waypointing")
	TArray<AWaypointNode*> Waypoints;
};
