﻿// Copyright (c) 2023, Stinky Cheese, All rights reserved.


#include "InspectableObject.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AInspectableObject::AInspectableObject()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectMesh"));
	RootComponent = ObjectMesh;

	holding = false;
}

// Called when the game starts or when spawned
void AInspectableObject::BeginPlay()
{
	Super::BeginPlay();

	OriginalLocation = GetActorLocation();
	OriginalRotation = GetActorRotation();

	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	PlayerCamera = PlayerCharacter->FindComponentByClass<UCameraComponent>();

	TArray<USceneComponent*> Components;

	PlayerCharacter->GetComponents(Components);

	if (Components.Num() > 0)
	{
		for (auto& Comp : Components)
		{
			if (Comp->GetName() == "HoldingComponent")
			{
				HoldingComponent = Cast<USceneComponent>(Comp);
			}
		}
	}
}

// Called every frame
void AInspectableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (holding && HoldingComponent)
	{
		SetActorLocationAndRotation(HoldingComponent->GetComponentLocation(), HoldingComponent->GetComponentRotation());
	}
	else if (!OriginalLocation.Equals(GetActorLocation(), 0.25f) && !OriginalRotation.Equals(GetActorRotation(), 0.25f))
	{
		//Slowly move it back to its original position
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), OriginalLocation, DeltaTime, ObjectMoveSpeed));
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), OriginalRotation, DeltaTime, ObjectMoveSpeed));
	}
}

void AInspectableObject::RotateActor()
{
	ControlRotation = GetWorld()->GetFirstPlayerController()->GetControlRotation();
	SetActorRotation(FQuat(ControlRotation));
}

void AInspectableObject::Pickup()
{
	holding = !holding;

	ObjectMesh->SetSimulatePhysics(holding ? false : true);
	ObjectMesh->SetCollisionEnabled(holding ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);

	if (!holding)
	{
		ForwardVector = PlayerCamera->GetForwardVector();
	}
}
