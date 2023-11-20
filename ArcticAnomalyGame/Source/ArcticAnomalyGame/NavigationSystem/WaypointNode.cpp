// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#include "WaypointNode.h"

// TODO: Give the waypoint node a non-collision collider component to make it easier to select in the editor.
// Sets default values.
AWaypointNode::AWaypointNode()
{
	// TODO: Disable ticks in game builds.
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create and attach a default scene root component.
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	
	// Set the root component as the RootComponent of the actor.
	RootComponent = Root;
	
	// Create a box component that does not collide or affect navmesh, this helps for editor selection.
	EditorBoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("EditorBoxCollider"));
	EditorBoxCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EditorBoxCollider->SetCanEverAffectNavigation(false);
	EditorBoxCollider->SetupAttachment(RootComponent);
	EditorBoxCollider->SetBoxExtent(FVector(DebugRadius, DebugRadius, DebugHeight), false);
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

void AWaypointNode::DebugVisuals() const
{
#if WITH_EDITOR
	if (!GWorld->IsGameWorld() || (GWorld->IsGameWorld() && bDebugInPlay))
	{
		FColor Color = IsSpawnPoint ? Color = FColor::Green : FColor::Cyan;
		
		// Draw a debug box around the waypoint node.
		DrawDebugCylinder(
			GetWorld(), GetActorLocation(), GetActorLocation() + FVector(0, 0, DebugRadius),
			DebugRadius, 12, Color, false, -1, 0, DebugThickness);
		
		// Draw a debug line between connected waypoints, accounting for null.
		for (int32 i = 0; i < ConnectedWaypoints.Num(); i++)
		{
			if (ConnectedWaypoints[i] != nullptr)
			{
				DrawDebugLine(
					GetWorld(), GetActorLocation(), ConnectedWaypoints[i]->GetActorLocation(),
					FColor::Cyan, false, -1, 0, DebugThickness);
			}
		}
	}
#endif
}

void AWaypointNode::ConnectWaypointsBidirectionally()
{
	// TODO: Implement logic to automatically link waypoints in both directions.
	UE_LOG(LogTemp, Warning, TEXT("ConnectWaypointsBidirectionally() called!"));
}

// TODO: Disable this method entirely in game builds.
bool AWaypointNode::ShouldTickIfViewportsOnly() const
{
	return true;
}

