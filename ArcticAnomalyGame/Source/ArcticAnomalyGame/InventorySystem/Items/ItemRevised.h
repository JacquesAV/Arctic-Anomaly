// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemRevised.generated.h"

UCLASS(BlueprintType)
class ARCTICANOMALYGAME_API UItemRevised : public UDataAsset
{
	GENERATED_BODY()

public:
	UItemRevised();
	
	virtual UWorld* GetWorld() const { return World; }

	UPROPERTY(Transient)
	UWorld* World;

	/**The text displayed when looking at the item(Pick up, swap etc)*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item")
	FText UseActionText;
	
	/**The mesh to display for this item when in the world*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item")
	UStaticMesh* PickupMesh;

	/**The thumbnail for this item*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item")
	UTexture2D* Thumbnail;

	/**The display name for this item in the inventory*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item");
	FText ItemDisplayName = FText::FromString("Item");

	/**The inventory this item is in*/
	UPROPERTY()
	class UInventoryComponent* OwningInventory;

	virtual void Use(class AArcticAnomalyGameCharacter* Character) PURE_VIRTUAL(UItem,);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUse(class AArcticAnomalyGameCharacter* Character);
};
