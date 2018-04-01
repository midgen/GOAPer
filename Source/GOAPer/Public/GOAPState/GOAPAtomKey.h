#pragma once
#include "GOAPAtomKey.generated.h"

USTRUCT(BlueprintType)
struct GOAPER_API FGOAPAtomKey
{
	GENERATED_USTRUCT_BODY()
public:
	/** Struct is required to be able to customise blueprint pins **/
	FGOAPAtomKey(uint8 aKey) : Key(aKey) {};
	FGOAPAtomKey() {};
	~FGOAPAtomKey() {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GOAP")
	uint8 Key;

};