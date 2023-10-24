// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#include "EnemyCharacter.h"
#include "NavigationPath.h"
#include "ArcticAnomalyGame/Interactables/BaseDoor.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	
	// Update the current target reference if it is null, default to the primary player character.
	if (!CurrentTarget)
	{
		CurrentTarget = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		if (CurrentTarget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Player character was fetched successfully!"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Player character could not be fetched!"));
		}
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
	
	TryOpenDoor();
	
	if (CurrentTarget && HasLineOfSight(CurrentTarget))
	{
		bIsCurrentlyChasing = true;
		ChaseTimeoutTimer = OutOfSightChaseTime;
	}
	else
	{
		ChaseTimeoutTimer -= DeltaTime;
		if (ChaseTimeoutTimer <= 0)
		{
			bIsCurrentlyChasing = false;
		}
	}
	
	if (bIsCurrentlyChasing)
	{
		GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
		ChaseTarget(CurrentTarget);
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
		FollowWaypoints();
	}
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyCharacter::ChaseTarget(const AActor* Target) const
{
	if(!Target)
	{
		return;
	}
	
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), Target);
	
	// Debug draw the path being followed.
	if (GEngine)
	{
		const UNavigationPath* NavPath = UAIBlueprintHelperLibrary::GetCurrentPath(GetController());
		TArray<FVector> Path = NavPath->PathPoints;
		
		for (int32 i = 0; i < Path.Num() - 1; ++i)
		{
			DrawDebugLine(GetWorld(), Path[i], Path[i + 1], FColor::Cyan, false, -1, 0, DebugThickness);
		}
	}
}

bool AEnemyCharacter::HasLineOfSight(AActor* Target)
{
	if(!Target)
	{
		return false;
	}
	
	bool bTargetInLOS = false;
	FColor DebugColor = FColor::Yellow;
	FVector StartLocation = GetActorLocation() + FVector(0, 0, DetectionHeightOffset);
	FVector EndLocation = Target->GetActorLocation() + FVector(0, 0, DetectionHeightOffset/2);
	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("LOS_Trace")), true, this);
	
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams))
	{
		// Debug log the name of the hit result.
		//UE_LOG(LogTemp, Warning, TEXT("Hit result: %s"), *HitResult.GetActor()->GetName());
		
		// Check if the hit actor is the target or if the hit component is part of the target.
		if (HitResult.GetActor() == Target || HitResult.Component->GetOwner() == Target)
		{
			DebugColor = FColor::Green;
			bTargetInLOS = true;
		}
	}
	
	// Debug the enemy's line of sight.
	if (GEngine)
	{
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, DebugColor, false, -1, 0, DebugThickness);
	}
	
	// Return true if the player is in LOS, false otherwise.
	return bTargetInLOS;
}

// TODO: Replace the sweep check with something simpler or something with less overhead.
void AEnemyCharacter::TryOpenDoor() const
{
	// Create a box-shaped collision channel to check if the enemy is looking at the door.
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	const FVector Start = GetActorLocation() + FVector(0, 0, DetectionHeightOffset);
	const FVector End = Start + GetActorForwardVector() * DoorDetectionDistance;
	const float HalfSideSize = DoorHandlingSideSize/2;
	const FVector HalfExtents = FVector(DoorDetectionDistance, HalfSideSize, HalfSideSize);
	TArray<FHitResult> HitResults;
	FColor DebugColor = FColor::Cyan;
	
	if (GetWorld()->SweepMultiByObjectType(HitResults, Start, End, FQuat::Identity, ECC_WorldDynamic, FCollisionShape::MakeBox(HalfExtents), QueryParams))
	{
		for (const FHitResult& HitResult : HitResults)
		{
			// Check if the hit component is the expected type.
			UPrimitiveComponent* HitComponent = HitResult.GetComponent();
			if (HitComponent->IsA(UBoxComponent::StaticClass()))
			{
				// Check for the expected name.
				UBoxComponent* Collider = Cast<UBoxComponent>(HitComponent);
				if (!Collider || Collider->GetName() != "Collider")
				{
					continue;
				}
				
				// Handle the door logic.
				AActor* HitActor = HitResult.GetActor();
				if (ABaseDoor* Door = Cast<ABaseDoor>(HitActor))
				{
					DebugColor = FColor::Orange;
					if (Door->isClosed)
					{
						Door->ForceOpenDoor(GetActorForwardVector());
						UE_LOG(LogTemp, Warning, TEXT("Enemy is opening door: %s"), *HitActor->GetName());
					}
				}
			}
		}
	}
	
	// Draw a debug box to visualize the box cast.
	FVector Center = Start + (GetActorForwardVector() * DoorDetectionDistance * 0.5f);
	FVector Extent = FVector(DoorDetectionDistance * 0.5f, HalfSideSize, HalfSideSize);
	FQuat RotationQuat = FQuat(GetActorForwardVector().Rotation());
	DrawDebugBox(GetWorld(), Center, Extent, RotationQuat, DebugColor, false, -1, 0, 5);
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
				if(bRespawnAfterPatrol)
				{
					// TODO: If the player is within LOS, continue patrolling until the player is out of LOS.
					RespawnLogic();
				}
				else
				{
					// If not respawning after patrolling, clean the visited nodes and return.
					// TODO: Have a visit count on the waypoints instead to avoid this issue and to make a more robust system.
					WaypointManager->ResetVisits();
				}
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
			UE_LOG(LogTemp, Warning, TEXT("Target waypoint is null, forcing a respawn!"));
			RespawnLogic();
		}
	}
}
