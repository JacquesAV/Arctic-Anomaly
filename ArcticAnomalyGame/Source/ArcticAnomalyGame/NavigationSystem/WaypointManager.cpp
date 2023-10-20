// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#include "WaypointManager.h"
#include "WaypointNode.h"

// Sets default values
AWaypointManager::AWaypointManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWaypointManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWaypointManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

AWaypointNode* AWaypointManager::GetNextWaypoint(const AWaypointNode* CurrentWaypoint) const
{
	if (!CurrentWaypoint)
	{
		// Handle the case where the input waypoint is invalid.
		return nullptr;
	}
	
	TArray<AWaypointNode*> UnvisitedConnectedWaypoints;
	
	// Fetch connected waypoints from the input waypoint.
	TArray<AWaypointNode*> ConnectedWaypoints = CurrentWaypoint->ConnectedWaypoints;
	
	// Iterate through the connected waypoints to find unvisited ones.
	for (AWaypointNode* Waypoint : ConnectedWaypoints)
	{
		if (!RecentlyVisitedNodes.Contains(Waypoint))
		{
			UnvisitedConnectedWaypoints.Add(Waypoint);
		}
	}
	
	// If there are unvisited connected waypoints, select one randomly.
	if (UnvisitedConnectedWaypoints.Num() > 0)
	{
		return GetRandomNode(UnvisitedConnectedWaypoints);
	}
	
	// If all connected waypoints are visited, select one randomly from all connected waypoints.
	if (ConnectedWaypoints.Num() > 0)
	{
		return GetRandomNode(ConnectedWaypoints);
	}
	
	// No connected waypoints are available.
	return nullptr;
}

// Reset the recently visited nodes array.
void AWaypointManager::ResetVisits()
{
	RecentlyVisitedNodes.Empty();
}

// Add a node to the recently visited nodes array.
void AWaypointManager::AddNodeToRecentlyVisited(AWaypointNode* NodeToAdd)
{
	if (NodeToAdd)
	{
		RecentlyVisitedNodes.Add(NodeToAdd);
	}
}

// Get a random spawn point outside a radius (if possible).
// TODO: Implement a raycast vision check to ensure the spawn point is not visible.
AWaypointNode* AWaypointManager::GetRandomSpawnPoint(const FVector& Location, const float Radius = 0) const
{
	TArray<AWaypointNode*> SpawnPointsOutsideRadius;

	// Iterate through the spawn points to find ones outside the radius.
	for (AWaypointNode* SpawnPoint : SpawnPoints)
	{
		// Skip this if the radius is 0.
		if (Radius == 0)
		{
			break;
		}

		if (FVector::Dist(Location, SpawnPoint->GetActorLocation()) >= Radius)
		{
			SpawnPointsOutsideRadius.Add(SpawnPoint);
		}
	}
	
	// If there are spawn points outside the radius, select one randomly.
	if (SpawnPointsOutsideRadius.Num() > 0)
	{
		return GetRandomNode(SpawnPointsOutsideRadius);
	}

	// If there are no spawn points outside the radius, select one randomly from all spawn points.
	return GetRandomSpawnPoint();
}

AWaypointNode* AWaypointManager::GetRandomSpawnPoint() const { return GetRandomNode(SpawnPoints); }

// Get random node using a given list.
AWaypointNode* AWaypointManager::GetRandomNode(const TArray<AWaypointNode*>& Nodes) const
{
	// If there are spawn points in the list, select one randomly.
	if (Nodes.Num() > 0)
	{
		const int32 RandomIndex = FMath::RandRange(0, Nodes.Num() - 1);
		return Nodes[RandomIndex];
	}
	
	// No spawn points are available.
	return nullptr;
}
