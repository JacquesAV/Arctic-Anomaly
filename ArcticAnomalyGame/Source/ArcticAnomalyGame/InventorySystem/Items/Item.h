// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Item.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class ARCTICANOMALYGAME_API UItem : public UObject
{
	GENERATED_BODY()

public:
	UItem();

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item")
	;
	FText ItemDisplayName;

	/**The inventory this item is in*/
	UPROPERTY()
	class UInventoryComponent* OwningInventory;

	virtual void Use(class AArcticAnomalyGameCharacter* Character) PURE_VIRTUAL(UItem,);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUse(class AArcticAnomalyGameCharacter* Character);
};
