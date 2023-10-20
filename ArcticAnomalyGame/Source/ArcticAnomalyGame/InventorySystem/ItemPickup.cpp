// Copyright (c) 2023, Stinky Cheese, All rights reserved.


#include "ItemPickup.h"
#include "Items/Item.h"

// Sets default values
AItemPickup::AItemPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create the box collider
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetGenerateOverlapEvents(true);
	BoxCollider->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AItemPickup::OnOverlapBegin);
	BoxCollider->AttachToComponent(this->RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

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

void AItemPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Check if the OtherActor is not self and is not null
	if(OtherActor!= nullptr && OtherActor != this && OtherComp != nullptr)
	{
		Destroy();
	}
}

void AItemPickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(AItemPickup, Item))
	{
		//Create the item mesh based on the UItem mesh
		if(Item && Item->PickupMesh)
		{
			// Create and attach a UStaticMeshComponent to represent the item's mesh
			ItemMesh->SetStaticMesh(Item->PickupMesh);
			ItemMesh->SetupAttachment(BoxCollider); // Attach to the PickupMesh
		}
	}
}

