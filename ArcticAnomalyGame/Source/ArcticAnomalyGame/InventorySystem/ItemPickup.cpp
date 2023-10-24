// Copyright (c) 2023, Stinky Cheese, All rights reserved.


#include "ItemPickup.h"
#include "Items/Item.h"

// Sets default values
AItemPickup::AItemPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent=CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	//Create the box collider
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetGenerateOverlapEvents(true);
	BoxCollider->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	BoxCollider->SetupAttachment(RootComponent);
	BoxCollider->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	ItemMesh->SetWorldScale3D(FVector(1.0f));
	ItemMesh->SetupAttachment(BoxCollider);
}

// Called when the game starts or when spawned
void AItemPickup::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AItemPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemPickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property)
	{
		//Create the item mesh based on the UItem mesh
		if(PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(AItemPickup, Item)&&Item && Item->PickupMesh)
		{
			// Create and attach a UStaticMeshComponent to represent the item's mesh
			ItemMesh->SetStaticMesh(Item->PickupMesh);
			ItemMesh->SetupAttachment(BoxCollider); // Attach to the PickupMesh
		}
	}
}

