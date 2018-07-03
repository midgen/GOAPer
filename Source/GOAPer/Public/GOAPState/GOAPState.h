#pragma once

#include "GOAPState.generated.h"

USTRUCT(BlueprintType)
struct GOAPER_API FGOAPState
{
	GENERATED_USTRUCT_BODY()
public:

	FGOAPState() {};
	~FGOAPState() {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOAP")
	TMap<uint8, bool> State;

	FGOAPState& operator+(const FGOAPState& aOther)
	{
		for (auto& elem : aOther.State)
		{
			State.Add(elem.Key, elem.Value);
		}
		return *this;
	}

	bool IsSatisfiesState(const FGOAPState& aOther) const
	{
		for (const auto& elem : aOther.State)
		{			
			if (State.Contains(elem.Key))
			{
				const bool* val = State.Find(elem.Key);
				if (*val != elem.Value)
				{
					return false;
				}
			}
		}
		return true;
	}

	bool IsStateSatisfied(const uint8 aState, const bool aValue) const
	{
		if (State.Contains(aState))
		{
			const bool* val = State.Find(aState);
			if (*val == aValue)
			{
				return true;
			}

		}
		return false;
	}

	void SetState(const uint8 aState, const bool aValue) 
	{
		State.Add(aState, aValue);
	}
};