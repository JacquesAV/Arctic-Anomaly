#pragma once
#include "Item.h"
#include "FItemBoolPair.generated.h"

UCLASS()
class UItemBoolPair : public UObject
{
	GENERATED_BODY()

public:
	UItemBoolPair():Key(nullptr),Value(false){}

	UItemBoolPair(UItem* InKey, bool InValue):Key(InKey),Value(InValue){}

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Item")
	UItem* Key;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Item")
	bool Value;
};

UCLASS()
class ARCTICANOMALYGAME_API UCustomDataManager : public UObject
{
	GENERATED_BODY()

public:
    UFUNCTION(Category = "Custom Data Manager")
	void InitializeValues();
	
	UFUNCTION(BlueprintCallable,Category = "Custom Data Manager")
	bool GetValueForKey(UItem* Key, bool& Value);

	UFUNCTION(BlueprintCallable,Category = "Custom Data Manager")
	bool SetValueForKey(UItem* Key, bool Value);

	UFUNCTION(BlueprintCallable,Category = "Custom Data Manager")
	void AddKeyValuePair(UItemBoolPair* NewPair);

	UFUNCTION(BlueprintCallable,Category = "Custom Data Manager")
	void AllValuesTrue(bool& AllTrue);

private:
	UPROPERTY(EditAnywhere, Category = "Custom Data Manager")
	TArray<UItemBoolPair*> KeyValuePairs;
	
};
