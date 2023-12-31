﻿// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	for (auto& Item : DefaultItems)
	{
		AddItem(Item);
	}
}

bool UInventoryComponent::AddItem(UItem* Item)
{
	//Currently no reason for it not to add an item but can return false if an item cannot be added

	Item->OwningInventory = this;
	Item->World = GetWorld();
	Items.Add(Item);
	//Log the item added
	UE_LOG(LogTemp, Warning, TEXT("Item added to inventory: %s"), *Item->ItemDisplayName.ToString());

	//Update UI
	OnInventoryUpdated.Broadcast();

	return true;
}

bool UInventoryComponent::RemoveItem(UItem* Item)
{
	if (Item)
	{
		Item->OwningInventory = nullptr;
		Item->World = nullptr;
		Items.RemoveSingle(Item);
		OnInventoryUpdated.Broadcast();
		return true;
	}

	return false;
}
