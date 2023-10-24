#include "FItemBoolPair.h"

void UCustomDataManager::InitializeValues()
{
	KeyValuePairs= TArray<UItemBoolPair*>();
}

bool UCustomDataManager::GetValueForKey(UItem* Key, bool& OutValue)
{
	for (const UItemBoolPair* Pair : KeyValuePairs)
	{
		if (Pair->Key == Key)
		{
			OutValue = Pair->Value;
			return true; // Key found, return the value.
		}
	}
	
	return false; // Key not found.
}

bool UCustomDataManager::SetValueForKey(UItem* Key, bool NewValue)
{
	for (UItemBoolPair* Pair : KeyValuePairs)
	{		
		//check if pair is not empty
		if (Pair->Key != nullptr &&Pair->Key->ItemDisplayName.EqualTo(Key->ItemDisplayName))
		{
			Pair->Value = NewValue;
			return true; // Key found, set the new value.
		}
	}
	return false; // Key not found.
}

void UCustomDataManager::AddKeyValuePair(UItemBoolPair* NewPair)
{
	KeyValuePairs.Add(NewPair);
}

void UCustomDataManager::AllValuesTrue(bool& AllTrue)
{
	for (const UItemBoolPair* Pair : KeyValuePairs)
	{
		if (!Pair->Value)
		{
			AllTrue = false;
			return;
		}
	}
	AllTrue = true;
}
