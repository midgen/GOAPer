#pragma once
#include "GOAPer.h"
#include "GOAPAtom.generated.h"

USTRUCT(BlueprintType)
struct GOAPER_API FGOAPAtom
{
	GENERATED_USTRUCT_BODY()
public:
	FGOAPAtom(uint8 aKey, bool aValue) : Key(aKey), Value(aValue) {};
	FGOAPAtom() {};
	~FGOAPAtom() {};
	bool operator==(const FGOAPAtom& aOther)
	{
		return aOther.Key == Key && aOther.Value == Value;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Key;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Value;
};