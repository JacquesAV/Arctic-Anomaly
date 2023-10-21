// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#include "EnemyCharacter.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values.
AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Access the capsule & mesh component.
	UCapsuleComponent* MyCapsuleComponent = GetCapsuleComponent();
	USkeletalMeshComponent* MyMeshComponent = GetMesh();

	// Set the relevant properties.
	if (MyCapsuleComponent && MyMeshComponent)
	{
		MyCapsuleComponent->SetCapsuleSize(34.f, 88.f);
		MyCapsuleComponent->ShapeColor = FColor::Green;
		MyCapsuleComponent->SetCollisionProfileName("CharacterMesh");
		RootComponent = MyCapsuleComponent;
		
		MyMeshComponent->SetupAttachment(RootComponent);
		MyMeshComponent->SetCanEverAffectNavigation(false);
	}
	
	// Create a capsule component and attach it to the root.
	TemporaryCapsuleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TemporaryCapsuleMesh"));
	TemporaryCapsuleMesh->SetCollisionProfileName("Trigger");
	TemporaryCapsuleMesh->SetupAttachment(RootComponent);
	TemporaryCapsuleMesh->SetCanEverAffectNavigation(false);
	
	// Disable gravity so the object doesnt fall out of the map.
	TemporaryCapsuleMesh->SetEnableGravity(false);
}

// Called when the game starts or when spawned.
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Update the waypoint manager reference if it is null.
	if (!WaypointManager)
	{
		WaypointManager = Cast<AWaypointManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AWaypointManager::StaticClass()));
		UE_LOG(LogTemp, Warning, TEXT("Waypoint manager had to be fetched, this should be set!"));
	}
	
	// Update the spawn points in the waypoint manager, reset the visits and move to the first waypoint.
	if (WaypointManager)
	{
		WaypointManager->UpdateSpawnPoints();
		RespawnLogic();
	}
	
	// Update the player character reference if it is null.
	if (!PlayerCharacter)
	{
		PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	}
}

void AEnemyCharacter::RespawnLogic()
{
	// Clean up the old visits.
	WaypointManager->ResetVisits();

	// Choose a random spawn point.
	// TODO: Use the random spawn point that accounts for the player's location.
	AWaypointNode* RandomSpawnPoint = WaypointManager->GetRandomSpawnPoint();
	WaypointManager->UpdateOriginWaypoint(RandomSpawnPoint);
	WaypointManager->UpdateTargetWaypoint(RandomSpawnPoint);

	// Teleport to the new spawn point.
	SetActorLocation(RandomSpawnPoint->GetActorLocation());
	bIsAwake = true;
}

// Called every frame.
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(!bIsAwake)
	{
		return;
	}
	
	// Check if the player is within the chase range
	if (IsPlayerInChaseRange())
	{
		//TODO: Implement chase logic.
	}
	else
	{
		FollowWaypoints();
	}
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

//TODO: Implement this function.
bool AEnemyCharacter::IsPlayerInChaseRange()
{
	return false;
}

void AEnemyCharacter::FollowWaypoints()
{
	// If the waypoint manager is valid, follow the waypoints.
	if (WaypointManager)
	{
		// If the target waypoint is reached, update the target waypoint.
		if (GetActorLocation().Equals(WaypointManager->TargetWaypoint->GetActorLocation(), 100))
		{
			// If the origin waypoint was reached (again), respawn instead.
			if (WaypointManager->TargetWaypoint == WaypointManager->OriginWaypoint && WaypointManager->RecentlyVisitedNodes.Num() > 0)
			{
				// TODO: If the player is within LOS, continue patrolling until the player is out of LOS.
				RespawnLogic();
				return;
			}

			// If then next waypoint is valid, update the target waypoint and add the current waypoint to the recently visited nodes.
			if (AWaypointNode* NextWaypoint = WaypointManager->GetNextWaypoint(WaypointManager->TargetWaypoint))
			{
				WaypointManager->AddNodeToRecentlyVisited(WaypointManager->TargetWaypoint);
				WaypointManager->UpdateTargetWaypoint(NextWaypoint);
			}
		}
		
		// Move to the target waypoint using navmesh pathfinding if it is valid.
		if (WaypointManager->TargetWaypoint)
		{
			UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), WaypointManager->TargetWaypoint);
		}
		// If the target waypoint is null, log a warning and try to respawn.
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Target waypoint is null!"));
			RespawnLogic();
		}
	}
}
