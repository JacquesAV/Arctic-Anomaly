// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#include "BaseDoor.h"
#include "Components/BoxComponent.h"

// Sets default values
ABaseDoor::ABaseDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BOX Comp"));
	BoxComp->InitBoxExtent(FVector(150, 100, 100));
	BoxComp->SetCollisionProfileName("Trigger");
	RootComponent = BoxComp;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DOOR"));
	DoorMesh->SetupAttachment(RootComponent);

	//Create a door asset
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetRelativeLocation(FVector(0.0f, 50.0f, -100.0f));
	DoorMesh->SetWorldScale3D(FVector(1.0f));
	DoorMesh->SetupAttachment(RootComponent);

	ColliderComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	ColliderComponent->SetupAttachment(DoorMesh);

	//Set the collider to prevent physics objects from moving through the door.
	ColliderComponent->SetCollisionResponseToAllChannels(ECR_Block);

	isClosed = true;
	Opening = false;
	Closing = false;
}

// Called when the game starts or when spawned
void ABaseDoor::BeginPlay()
{
	Super::BeginPlay();

	DrawDebugBox(GetWorld(), GetActorLocation(), BoxComp->GetScaledBoxExtent(), FQuat(GetActorRotation()),
	             FColor::Turquoise, true, -1, 0, 2);
}

// Called every frame
void ABaseDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Opening)
	{
		OpenDoor(DeltaTime);
	}

	if (Closing)
	{
		CloseDoor(DeltaTime);
	}
}

void ABaseDoor::OpenDoor(float deltaTime)
{
}

void ABaseDoor::CloseDoor(float deltaTime)
{
}

void ABaseDoor::ToggleDoor(FVector ForwardVector)
{
	if (isClosed)
	{
		isClosed = false;
		Closing = false;
		Opening = true;
	}
	else
	{
		Opening = false;
		Closing = true;
		isClosed = true;
	}
}

void ABaseDoor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Check if the property that was changed is the static mesh component
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(ABaseDoor, DoorMesh))
	{
		FBoxSphereBounds MeshBounds = DoorMesh->GetStaticMesh()->GetBounds();
		FVector MeshExtent = MeshBounds.BoxExtent;
		ColliderComponent->SetBoxExtent(MeshExtent);
		
		ColliderComponent->SetRelativeLocation(FVector(0.0f, -MeshExtent.Y, MeshExtent.Z));
	}
}
