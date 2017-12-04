#pragma once

#include "GOAPerSettings.generated.h"


/**
 * Implements the settings for the GOAPer Plugin
 */
UCLASS(config=GOAPer, BlueprintType)
class GOAPER_API UGOAPerSettings
	: public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category=State)
	TArray<FString> AvailableStates;

	UFUNCTION(BlueprintPure, Category = GOAPer)
	TArray<FString> GetStateSettings();

	TSharedPtr<FString> GetStringForByte(const uint8 inByte);
	uint8 GetByteKey(TSharedPtr<FString> inString);
};
