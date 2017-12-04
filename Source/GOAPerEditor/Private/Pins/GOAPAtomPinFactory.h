#pragma once
#include "SlateBasics.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_K2.h"
#include "GOAPAtomKey.h"
#include "GOAPAtomPin.h"
#include "EdGraphUtilities.h"

class FGOAPAtomPinFactory : public FGraphPanelPinFactory
{
	// Registers our custom pin type and generates as needed
	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* InPin) const override
	{
		const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

		if (InPin->PinType.PinCategory == K2Schema->PC_Struct
			&& InPin->PinType.PinSubCategoryObject == FGOAPAtomKey::StaticStruct())
		{
			return SNew(SGOAPAtomPin, InPin); 
		}
		return nullptr;
	}
};