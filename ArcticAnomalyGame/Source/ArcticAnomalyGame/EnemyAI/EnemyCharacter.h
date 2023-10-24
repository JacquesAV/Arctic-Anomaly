// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ArcticAnomalyGame/NavigationSystem/WaypointManager.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class ARCTICANOMALYGAME_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void RespawnLogic();

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void ChaseTarget(const AActor* Target) const;
	bool HasLineOfSight(AActor* Target) const;
	void ForwardHitSweepCheck();
	bool HitResultPlayerCheck(const FHitResult* HitResult);
	bool HitResultDoorCheck(const FHitResult* HitResult) const;
	void FollowWaypoints();
	
	UPROPERTY(EditAnywhere, Category = "Management")
	bool bIsAwake = false;
	
	UPROPERTY(EditAnywhere, Category = "Management")
	bool bRespawnAfterPatrol = true;

	UPROPERTY(VisibleAnywhere, Category = "Management")
	bool bIsCurrentlyChasing = false;

	UPROPERTY(EditAnywhere, Category = "Management")
	float ChaseTimeoutTimer = 0;

	UPROPERTY(EditAnywhere, Category = "Management")
	ACharacter *CurrentTarget;

	UPROPERTY(EditAnywhere, Category = "Statistics")
	int PatrolSpeed = 200;

	UPROPERTY(EditAnywhere, Category = "Statistics")
	int ChaseSpeed = 625;

	UPROPERTY(EditAnywhere, Category = "Detection")
	float OutOfSightChaseTime = 2;
	
	UPROPERTY(EditAnywhere, Category = "Detection")
	int DetectionHeightOffset = 100;
	
	UPROPERTY(EditAnywhere, Category = "Door Handling")
	int DoorDetectionDistance = 100;
	
	UPROPERTY(EditAnywhere, Category = "Door Handling")
	int DoorHandlingSideSize = 100;
	
	UPROPERTY(EditAnywhere, Category = "Management")
	AWaypointManager* WaypointManager;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* TemporaryCapsuleMesh;
	
	UPROPERTY(EditAnywhere, Category = "Debugging")
	float DebugThickness = 5;
};
