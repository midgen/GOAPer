#include "GOAPerEditor.h"
#include "GOAPStateCustomization.h"
#include "GOAPState.h"
#include "STextComboBox.h"
#include "SCheckBox.h"
#include "Editor/PropertyEditor/Public/PropertyHandle.h"
#include "Editor/PropertyEditor/Public/DetailWidgetRow.h"
#include "IPropertyUtilities.h"

#define LOCTEXT_NAMESPACE "GOAPStateCustomization"


TSharedRef<IPropertyTypeCustomization> FGOAPStateCustomization::MakeInstance()
{
	return MakeShareable(new FGOAPStateCustomization());
}

void FGOAPStateCustomization::CustomizeHeader(TSharedRef<class IPropertyHandle> inStructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	StructPropertyHandle = inStructPropertyHandle;

	// Get the available options from our settings class
	for (auto& stateString : GOAPSettings->AvailableStates)
	{
		AvailableOptions.Add(MakeShareable<FString>(new FString(stateString)));
	}

	// Now fetch the handles to our child properties, and get their values
	uint32 NumChildren;
	StructPropertyHandle->GetNumChildren(NumChildren);

	for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
	{
		const TSharedPtr< IPropertyHandle > ChildHandle = StructPropertyHandle->GetChildHandle(ChildIndex);
		if (ChildHandle->GetProperty()->GetName() == TEXT("Key"))
		{
			KeyHandle = ChildHandle;
			ChildHandle->GetValue(Key);
		}
		if (ChildHandle->GetProperty()->GetName() == TEXT("Value"))
		{
			ValueHandle = ChildHandle;
			ChildHandle->GetValue(Value);
		}

	}

	// Make sure if we've changed options we don't go out of bounds
	if (Key >= AvailableOptions.Num())
	{
		Key = 0;
	}
	
	SelectedString = GOAPSettings->GetStringForByte(Key);

	check(KeyHandle.IsValid());
	check(ValueHandle.IsValid());

	HeaderRow
		.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(500)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			[
				SAssignNew(KeyComboBox, STextComboBox)
				.OptionsSource(&AvailableOptions)
				.OnSelectionChanged(this, &FGOAPStateCustomization::OnStateValueChanged)
				.InitiallySelectedItem(AvailableOptions[Key])
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			[
				SAssignNew(ValueCheckBox, SCheckBox)
				.OnCheckStateChanged(this, &FGOAPStateCustomization::OnCheckStateChanged)
				.IsChecked((ECheckBoxState)Value)
			]
		];


}


void FGOAPStateCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> inStructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	//Create further customization here
}

void FGOAPStateCustomization::OnStateValueChanged(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo)
{
	if (ItemSelected.IsValid())
	{
		// Fetches the byte index for this string from the settings class
		for (int32 i = 0; i < AvailableOptions.Num(); ++i)
		{
			if (AvailableOptions[i] == ItemSelected)
			{
				Key = GOAPSettings->GetByteKey(ItemSelected);
				KeyHandle->SetValue(Key);
				//SelectedString = ItemSelected;
			}
		}
	}
}

void FGOAPStateCustomization::OnStateListOpened()
{

}

void FGOAPStateCustomization::OnCheckStateChanged(ECheckBoxState CheckState)
{
	if (CheckState == ECheckBoxState::Checked)
	{
		ValueHandle->SetValue(true);
	}
	else
	{
		ValueHandle->SetValue(false);
	}
}

#undef LOCTEXT_NAMESPACE