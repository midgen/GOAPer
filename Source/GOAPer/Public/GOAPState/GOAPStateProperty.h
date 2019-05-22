#pragma once

#include "CoreMinimal.h"
#include "GOAPStateProperty.generated.h"

USTRUCT(BlueprintType)
struct GOAPER_API FGOAPStateProperty
{
	GENERATED_USTRUCT_BODY()
public:
	FGOAPStateProperty(uint8 aKey, bool aValue) : Key(aKey), Value(aValue) {};
	FGOAPStateProperty() {};
	~FGOAPStateProperty() {};
	bool operator==(const FGOAPStateProperty& aOther)
	{
		return aOther.Key == Key && aOther.Value == Value;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GOAP")
	uint8 Key;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GOAP")
	bool Value;
};