#pragma once

#include "GOAPAction.h"
#include "GOAPEdge.generated.h"

struct FGOAPNode;

USTRUCT()
struct GOAPER_API FGOAPEdge
{
	GENERATED_USTRUCT_BODY()
public:

	FGOAPEdge() {};
	~FGOAPEdge() {};

	TWeakObjectPtr<UGOAPAction> Action;
	FGOAPNode* Node = nullptr;

};