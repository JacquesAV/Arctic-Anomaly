// Copyright (c) 2023, Stinky Cheese, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "UObject/Object.h"
#include "KeyItem.generated.h"

UCLASS()
class ARCTICANOMALYGAME_API UKeyItem : public UItem
{
	GENERATED_BODY()

protected:
	virtual void Use(AArcticAnomalyGameCharacter* Character) override;
};
