// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#include "WaypointManager.h"
#include "WaypointNode.h"
#include "Kismet/GameplayStatics.h"

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

void AWaypointManager::UpdateSpawnPoints()
{
	// Fetch all waypoint nodes in the level.
	TArray<AActor*> WaypointNodes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaypointNode::StaticClass(), WaypointNodes);

	// Iterate through the waypoint nodes to find spawn points.
	for (AActor* WaypointNode : WaypointNodes)
	{
		if (AWaypointNode* WaypointNodeCasted = Cast<AWaypointNode>(WaypointNode))
		{
			if (WaypointNodeCasted->IsSpawnPoint)
			{
				SpawnPoints.Add(WaypointNodeCasted);
			}
		}
	}
}

// TODO: Implement improved logic to potentially ignore hanging nodes with no further connected waypoints.
// TODO: This would make the AI more "intelligent" and less likely to make odd loops over enough time.
AWaypointNode* AWaypointManager::GetNextWaypoint(const AWaypointNode* CurrentWaypoint) const
{
	if (!CurrentWaypoint)
	{
		// Handle the case where the input waypoint is invalid.
		return nullptr;
	}
	
	// Create arrays to store waypoints that will be managed shortly.
	TArray<FVisitedWaypointHolder> VisitedConnectedWaypoints;
	TArray<AWaypointNode*> UnvisitedConnectedWaypoints;
	
	// Fetch connected waypoints from the input waypoint.
	TArray<AWaypointNode*> ConnectedWaypoints = CurrentWaypoint->ConnectedWaypoints;
	
	// Iterate through the connected waypoints to find unvisited ones.
	for (AWaypointNode* CheckedWaypoint : ConnectedWaypoints)
	{
		// Check if the waypoint has already been visited recently.
		bool bContainsWaypoint = false;
		for (FVisitedWaypointHolder VisitedWaypointHolder : RecentlyVisitedNodes)
		{
			if(VisitedWaypointHolder.Waypoint == CheckedWaypoint && VisitedWaypointHolder.Waypoint != CurrentWaypoint)
			{
				VisitedConnectedWaypoints.Add(VisitedWaypointHolder);
				bContainsWaypoint = true;
				break;
			}
		}
		
		// Add if the waypoint is not in the recently visited nodes array.
		if(!bContainsWaypoint)
		{
			UnvisitedConnectedWaypoints.Add(CheckedWaypoint);
		}
	}
	
	// If there are unvisited connected waypoints, select one randomly.
	if (UnvisitedConnectedWaypoints.Num() > 0)
	{
		return GetRandomNode(UnvisitedConnectedWaypoints);
	}
	
	// If there are no unvisited connected waypoints, sort the recently visited nodes array and select the lowest.
	if (VisitedConnectedWaypoints.Num() > 0)
	{
		VisitedConnectedWaypoints.Sort(SortByLowestVisit);
		return VisitedConnectedWaypoints[0].Waypoint;
	}
	
	// If the previous two conditions are not met, default to a random connected waypoint and throw a warning.
	if (CurrentWaypoint->ConnectedWaypoints.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Encountered issue in waypoint fetching for waypoint %s, defaulting to random connected waypoint!"), *CurrentWaypoint->GetName());
		return GetRandomNode(ConnectedWaypoints);
	}
	
	// No connected waypoints are available.
	UE_LOG(LogTemp, Warning, TEXT("Encountered issue in waypoint fetching for waypoint %s, returning with nullptr!"), *CurrentWaypoint->GetName());
	return nullptr;
}

// Returns true if ItemA has less visits than ItemB, with some randomness for equal visit counts.
bool AWaypointManager::SortByLowestVisit(const FVisitedWaypointHolder& ItemA, const FVisitedWaypointHolder& ItemB)
{
	// If visit counts are equal, introduce randomness by shuffling.
	if (ItemA.VisitCount == ItemB.VisitCount)
	{
		return FMath::RandBool();
	}
	
	// Compare visit counts and choose the lowest.
	return ItemA.VisitCount < ItemB.VisitCount;
}

// Update the target waypoint.
void AWaypointManager::UpdateTargetWaypoint(AWaypointNode* NewTargetWaypoint)
{
	TargetWaypoint = NewTargetWaypoint;
}

// Update the origin waypoint.
void AWaypointManager::UpdateOriginWaypoint(AWaypointNode* NewOriginWaypoint)
{
	OriginWaypoint = NewOriginWaypoint;
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
		// Check if the waypoint has already been visited recently.
		for (FVisitedWaypointHolder& VisitedWaypointHolder : RecentlyVisitedNodes)
		{
			if(VisitedWaypointHolder.Waypoint == NodeToAdd)
			{
				VisitedWaypointHolder.VisitCount++;
				return;
			}
		}
		// Otherwise, add it to the array.
		RecentlyVisitedNodes.Add(FVisitedWaypointHolder{NodeToAdd, 1});
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
