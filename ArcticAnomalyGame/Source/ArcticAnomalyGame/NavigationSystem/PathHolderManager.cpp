// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#include "PathHolderManager.h"
#include "WaypointNode.h"

// Sets default values
APathHolderManager::APathHolderManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// Create and attach a default scene root component.
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	
	// Set the root component as the RootComponent of the actor.
	RootComponent = Root;
}

// TODO: Method is slightly inefficient, but it's only called in-editor, so it's likely fine.
void APathHolderManager::ConnectWaypointsBidirectionally() const
{
	UE_LOG(LogTemp, Display, TEXT("(1/2) Connecting %d waypoints bidirectionally for path %s..."), Waypoints.Num(), *Name);
	
	// Iterate through the waypoints and manage bidirectional linking.
	for (AWaypointNode* Waypoint : Waypoints)
	{
		UE_LOG(LogTemp, Display, TEXT("Connecting %s bidirectionally"), *Waypoint->GetName());
		
		for (AWaypointNode* ConnectedWaypoint : Waypoint->ConnectedWaypoints)
		{
			if (!ConnectedWaypoint->ConnectedWaypoints.Contains(Waypoint))
			{
				ConnectedWaypoint->ConnectedWaypoints.Add(Waypoint);
			}
		}
	}
	UE_LOG(LogTemp, Display, TEXT("(2/2) Finished connecting %d waypoints bidirectionally for path %s..."), Waypoints.Num(), *Name);
}

void APathHolderManager::UpdatePathWaypoints()
{
	UE_LOG(LogTemp, Display, TEXT("(1/2) Updating and fetching waypoints for path %s..."), *Name);
	
	// Fetch all waypoint actors attached to this actor and update the Waypoints array.
	TArray<AActor*> FoundActors;
	GetAttachedActors(FoundActors);
	Waypoints.Empty();
	for (AActor* FoundActor : FoundActors)
	{
		if (FoundActor->IsA(AWaypointNode::StaticClass()))
		{
			Waypoints.Add(Cast<AWaypointNode>(FoundActor));
			UE_LOG(LogTemp, Display, TEXT("Found actor: %s"), *FoundActor->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Found actor that was not a WaypointNode: %s"), *FoundActor->GetName());
		}
	}
	UE_LOG(LogTemp, Display, TEXT("(2/2) Finished updating and fetching %d waypoints for path %s..."), Waypoints.Num(), *Name);
}
