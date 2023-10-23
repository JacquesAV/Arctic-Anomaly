#pragma once
#include "Item.h"
#include "FItemBoolPair.generated.h"

USTRUCT(BlueprintType)
struct FItemBoolPair
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Item")
	UItem* Key;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Item")
	bool Value;

	FItemBoolPair()
	{
		Key=nullptr;
		Value=false;
	}

	FItemBoolPair(UItem* InKey, bool InValue)
	{
		Key=InKey;
		Value=InValue;
	}
};

UCLASS()
class ARCTICANOMALYGAME_API UCustomDataManager : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable,Category = "Custom Data Manager")
	bool GetValueForKey(UItem* Key, bool& Value);

	UFUNCTION(BlueprintCallable,Category = "Custom Data Manager")
	bool SetValueForKey(UItem* Key, bool Value);

	UFUNCTION(BlueprintCallable,Category = "Custom Data Manager")
	void AddKeyValuePair(FItemBoolPair NewPair);

	UFUNCTION(BlueprintCallable,Category = "Custom Data Manager")
	void AllValuesTrue(bool& AllTrue);

private:
	UPROPERTY(EditAnywhere, Category = "Custom Data Manager")
	TArray<FItemBoolPair> KeyValuePairs;
	
};
