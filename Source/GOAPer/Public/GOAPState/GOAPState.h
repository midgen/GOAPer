#pragma once

#include "CoreMinimal.h"
#include "GOAPState.generated.h"


// World state is represented

typedef TPair<uint8, bool> TWorldProperty;

USTRUCT(BlueprintType)
struct GOAPER_API FGOAPState
{
	GENERATED_USTRUCT_BODY()
public:

	FGOAPState() {};
	~FGOAPState() {};

	TArray<TWorldProperty> State;

	FGOAPState& operator+=(const FGOAPState& aOther)
	{
		for (const TWorldProperty& elem : aOther.State)
		{
			SetState(elem.Key, elem.Value);
		}
		return *this;
	}

	bool SatisfiesState(const FGOAPState& aOther) const
	{
		for (const TWorldProperty& otherItem : aOther.State)
		{			
			for (const TWorldProperty& item : State)
			{
				if (item.Key == otherItem.Key)
				{
					if (item.Value != otherItem.Value)
						return false;
				}
			}
		}
		return true;
	}

	bool SatisfiesProperty(const uint8 aPropertyId, const bool aValue) const
	{
		for (const TWorldProperty& item : State)
		{
			if (item.Key == aPropertyId)
			{
				if (item.Value == aValue)
				{
					return true;
				}
				else
				{
					return false;
				}

			}
		}

		return false;
	}

	void SetState(const uint8 aPropertyId, const bool aValue) 
	{
		for (TWorldProperty& item : State)
		{
			if (item.Key == aPropertyId)
			{
				item.Value = aValue;
				return;
			}
		}

		State.Add(TWorldProperty(aPropertyId, aValue));
	}

	bool GetPropertyValue(const uint8 aPropertyId) const
	{
		for (const TWorldProperty& item : State)
		{
			if (item.Key == aPropertyId)
			{
				return item.Value;
			}
		}

		return false;
	}
};