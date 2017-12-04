#pragma once
#include "GOAPAtom.h"
#include "GOAPStateUI.generated.h"

// We have a different struct for configuration purposes
//  so that we can specify our custom configuration panels
USTRUCT(BlueprintType)
struct GOAPER_API FGOAPStateUI
{
	GENERATED_USTRUCT_BODY()
public:

	FGOAPStateUI() {};
	~FGOAPStateUI() {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGOAPAtom> State;

};