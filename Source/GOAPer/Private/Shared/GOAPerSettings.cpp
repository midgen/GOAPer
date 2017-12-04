#pragma once
#include "GOAPer.h"
#include "GOAPerSettings.h"


/**
 * Implements the settings for the GOAPer Plugin
 */

TArray<FString> UGOAPerSettings::GetStateSettings()
{
	return AvailableStates;
}

TSharedPtr<FString> UGOAPerSettings::GetStringForByte(const uint8 inByte)
{
	TSharedPtr<FString> result;
	if (inByte < AvailableStates.Num())
	{
		result = MakeShareable<FString>(new FString(AvailableStates[inByte]));

	}
	else {
		result = MakeShareable<FString>(new FString(TEXT("Invalid")));
	}

	return result;

}

uint8 UGOAPerSettings::GetByteKey(TSharedPtr<FString> inString)
{
	for (uint8 i = 0; i < AvailableStates.Num(); ++i)
	{
		if (inString->Compare(AvailableStates[i]) == 0)
		{
			return i;
		}
	}

	return 0;
}
