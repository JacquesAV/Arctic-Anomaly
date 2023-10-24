// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item.h"
#include "InventoryComponent.generated.h"

//Classes can listen to this delegate and know when it has been called and then do something
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdate);//Blueprints can bind to this to update the UI

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARCTICANOMALYGAME_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	bool AddItem(UItem* Item);
	bool RemoveItem(UItem* Item);//Useful for the extinguisher probably

	/**The items the character will start with*/
	UPROPERTY(EditDefaultsOnly,Instanced)
	TArray<UItem*> DefaultItems;

	UPROPERTY(BlueprintAssignable,Category="Inventory")
	FOnInventoryUpdate OnInventoryUpdated;

	/*The current items in the player's inventory*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Inventory")
	TArray<UItem*> Items;
	
};
