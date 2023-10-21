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
	static bool IsPlayerInChaseRange();
	void FollowWaypoints();

	UPROPERTY(VisibleAnywhere, Category = "Management")
	bool bIsAwake = false;

	UPROPERTY(EditAnywhere, Category = "Management")
	AWaypointManager* WaypointManager;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* TemporaryCapsuleMesh;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	ACharacter *PlayerCharacter;
};
