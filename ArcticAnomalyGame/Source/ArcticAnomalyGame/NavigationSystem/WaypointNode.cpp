// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#include "WaypointNode.h"

// Sets default values.
AWaypointNode::AWaypointNode()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create and attach a default scene root component.
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	
	// Set the root component as the RootComponent of the actor.
	RootComponent = Root;
}

// Called when the game starts or when spawned.
void AWaypointNode::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame.
void AWaypointNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DebugVisuals();
}

#if WITH_EDITOR
void AWaypointNode::DebugVisuals() const
{
	if (!GWorld->HasBegunPlay())
	{
		FColor Color = IsSpawnPoint ? Color = FColor::Green : FColor::Turquoise;
		
		// Draw a debug box around the waypoint node.
		DrawDebugCylinder(
			GetWorld(), GetActorLocation(), GetActorLocation() + FVector(0, 0, 100),
			50, 12, Color, false, -1, 0, 2);
		
		// Draw a debug line between connected waypoints, accounting for null.
		for (int32 i = 0; i < ConnectedWaypoints.Num(); i++)
		{
			if (ConnectedWaypoints[i] != nullptr)
			{
				DrawDebugLine(
					GetWorld(), GetActorLocation(), ConnectedWaypoints[i]->GetActorLocation(),
					FColor::Emerald, false, -1, 0, 4);
			}
		}
	}
}

bool AWaypointNode::ShouldTickIfViewportsOnly() const
{
	return true;
}
#endif
