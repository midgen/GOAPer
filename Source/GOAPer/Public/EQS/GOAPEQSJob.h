#pragma once
#include "GOAPer.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "GOAPEQSJob.generated.h"

/** Struct defining an EQS job that actions can send to the controller to process */
USTRUCT(BlueprintType)
struct GOAPER_API FGOAPEQSJob
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	TWeakObjectPtr<UGOAPAction> CallingAction;

	UPROPERTY()
	UEnvQuery* Query;

	UPROPERTY()
	TEnumAsByte<EEnvQueryRunMode::Type> RunMode;
};