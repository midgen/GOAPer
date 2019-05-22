#pragma once

#include "CoreMinimal.h"
#include "GOAPPropertyId.generated.h"

USTRUCT(BlueprintType)
struct GOAPER_API FGOAPPropertyId
{
	GENERATED_USTRUCT_BODY()

	/** Struct is required to be able to customise blueprint pins **/
	FGOAPPropertyId(uint8 aKey) : Key(aKey) {};
	FGOAPPropertyId() {};
	~FGOAPPropertyId() {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GOAP")
	uint8 Key;

};