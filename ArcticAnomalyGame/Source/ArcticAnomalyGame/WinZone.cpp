// Copyright (c) 2023, Stinky Cheese, All rights reserved.


#include "WinZone.h"
#include "Components/BoxComponent.h"

// Sets default values
AWinZone::AWinZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	//create a box component
	UBoxComponent* WinZoneBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WinZoneBox"));
	WinZoneBox->SetCollisionProfileName(TEXT("WinZone"));
	WinZoneBox->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AWinZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWinZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

