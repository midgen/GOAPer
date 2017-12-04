#include "GOAPerEditor.h"
#include "KismetEditorUtilities.h"
#include "STextComboBox.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "GOAPAtom.h"
#include "GOAPAtomPin.h"

void SGOAPAtomPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{

	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

TSharedRef<SWidget>	SGOAPAtomPin::GetDefaultValueWidget()
{
	for (auto& stateString : GOAPSettings->AvailableStates)
	{
		AvailableOptions.Add(MakeShareable<FString>(new FString(stateString)));
	}
	// Apparently this is how you deal with pin defaults. IT IS INSANE
	FString CurrentDefault = GraphPinObj->GetDefaultAsString();
	if (CurrentDefault.Len() > 0)
	{
		int32 StartIndex = 5;
		int32 EndIndex;
		CurrentDefault.FindLastChar(')', EndIndex);
		FString DefaultValString = CurrentDefault.Mid(StartIndex, EndIndex);
		Key = FCString::Atoi(*DefaultValString);
	}
	else {
		Key = 0;
	}
	// Make sure if we've changed options we don't go out of bounds
	if (Key >= AvailableOptions.Num())
	{
		Key = 0;
	}

	//Creating the button that adds a new item on the list when pressed
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		[
			SNew(STextComboBox)
			.OptionsSource(&AvailableOptions)
			.OnSelectionChanged(this, &SGOAPAtomPin::OnStateValueChanged)
			.InitiallySelectedItem(AvailableOptions[Key])
		];
}

void SGOAPAtomPin::OnStateValueChanged(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo)
{
	if (ItemSelected.IsValid())
	{
		// Fetches the byte index for this string from the available options
		for (int32 i = 0; i < AvailableOptions.Num(); ++i)
		{
			if (AvailableOptions[i] == ItemSelected)
			{
				Key = GOAPSettings->GetByteKey(ItemSelected);
				
			}
		}
	}
	SetValue(Key);
}



void SGOAPAtomPin::SetValue(uint8 aKey)
{
	// Set the default
	FString strKey;
	strKey.AppendInt(aKey);

	FString KeyString = TEXT("(");

	KeyString += TEXT("Key=");
	KeyString += strKey;
	KeyString += TEXT(")");

	GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, KeyString);


}
