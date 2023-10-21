// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Items/Item.h"
#include "ItemPickup.generated.h"

UCLASS()
class ARCTICANOMALYGAME_API AItemPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category="Item")
	UItem* Item;

	UPROPERTY()
	UStaticMeshComponent* ItemMesh;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxCollider;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
};
