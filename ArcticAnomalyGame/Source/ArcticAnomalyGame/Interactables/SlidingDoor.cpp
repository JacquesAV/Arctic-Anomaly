// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#include "SlidingDoor.h"
//#include "ConstructorHelpers.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

// Sets default values
ASlidingDoor::ASlidingDoor()
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
	DoorMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetRelativeLocation(FVector(0.0f, 50.0f, -100.0f));
	DoorMesh->SetWorldScale3D(FVector(1.0f));
	DoorMesh->SetupAttachment(RootComponent);

	isClosed = true;
	Opening = false;
	Closing = false;

	DotProduct = 0.0f;
	MaxDegree = 0.0f;
	PosNeg = 0.0f;
	DoorCurrentRotation = 0.0f;
}

// Called when the game starts or when spawned
void ASlidingDoor::BeginPlay()
{
	Super::BeginPlay();

	DrawDebugBox(GetWorld(), GetActorLocation(), BoxComp->GetScaledBoxExtent(), FQuat(GetActorRotation()),
	             FColor::Turquoise, true, -1, 0, 2);
}

// Called every frame
void ASlidingDoor::Tick(float DeltaTime)
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

void ASlidingDoor::OpenDoor(float deltaTime)
{
	DoorCurrentRotation = DoorMesh->GetRelativeRotation().Yaw;

	AddRotation = PosNeg * deltaTime * 80;

	if (FMath::IsNearlyEqual(DoorCurrentRotation, MaxDegree, 1.5f))
	{
		Closing = false;
		Opening = false;
	}
	else if (Opening)
	{
		FRotator newRotation = FRotator(0.0f, AddRotation, 0.0f);
		DoorMesh->AddRelativeRotation(FQuat(newRotation), false, 0, ETeleportType::None);
	}
}

void ASlidingDoor::CloseDoor(float deltaTime)
{
	DoorCurrentRotation = DoorMesh->GetRelativeRotation().Yaw;

	if (DoorCurrentRotation > 0)
	{
		AddRotation = -deltaTime * 80;
	}
	else
	{
		AddRotation = deltaTime * 80;
	}

	if (FMath::IsNearlyEqual(DoorCurrentRotation, 0.0f, 1.5f))
	{
		Closing = false;
		Opening = false;
	}
	else if (Closing)
	{
		FRotator newRotation = FRotator(0.0f, AddRotation, 0.0f);
		DoorMesh->AddRelativeRotation(FQuat(newRotation), false, 0, ETeleportType::None);
	}
}

void ASlidingDoor::ToggleDoor(FVector ForwardVector)
{
	UE_LOG(LogTemp, Warning, TEXT("Door is opening2"));
	
	DotProduct = FVector::DotProduct(BoxComp->GetForwardVector(), ForwardVector);

	PosNeg = FMath::Sign(DotProduct);

	MaxDegree = PosNeg * 90;

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
