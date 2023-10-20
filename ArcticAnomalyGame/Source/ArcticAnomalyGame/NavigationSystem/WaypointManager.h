#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaypointNode.h"
#include "WaypointManager.generated.h"

UCLASS()
class ARCTICANOMALYGAME_API AWaypointManager : public AActor
{
	GENERATED_BODY()

public:
	// Constructor
	AWaypointManager();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Target waypoint for the AI to navigate to.
	UPROPERTY(EditAnywhere, Category = "Waypointing")
	AWaypointNode* TargetWaypoint;
	
	// Array to store references to spawn points.
	UPROPERTY(EditAnywhere, Category = "Waypointing")
	TArray<AWaypointNode*> SpawnPoints;

	// Array to store references to spawn points.
	UPROPERTY(EditAnywhere, Category = "Waypointing")
	TArray<AWaypointNode*> RecentlyVisitedNodes;
	
	// Function to get the next waypoint based on your logic
	UFUNCTION(BlueprintCallable, Category = "Waypointing")
	AWaypointNode* GetNextWaypoint(const AWaypointNode* CurrentWaypoint) const;
	
	void ResetVisits();
	void AddNodeToRecentlyVisited(AWaypointNode* NodeToAdd);
	
	AWaypointNode* GetRandomSpawnPoint(const FVector& Location, float Radius) const;
	AWaypointNode* GetRandomSpawnPoint() const;
	AWaypointNode* GetRandomNode(const TArray<AWaypointNode*>& Nodes) const;
};
