#include "Editor/DetailCustomizations/Public/DetailCustomizations.h"
#include "Editor/PropertyEditor/Public/IPropertyTypeCustomization.h"
#include "GOAPState.h"
#include "GOAPerSettings.h"
#pragma once

class FGOAPStateCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<class IPropertyHandle> inStructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> inStructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;


private:
	UGOAPerSettings* GOAPSettings = GetMutableDefault<UGOAPerSettings>();

	TArray<TSharedPtr<FString>> AvailableOptions;

	TSharedPtr<IPropertyHandle> StructPropertyHandle;

	TSharedPtr<IPropertyHandle> KeyHandle;
	TSharedPtr<IPropertyHandle> ValueHandle;

	TSharedPtr<STextComboBox> KeyComboBox;
	TSharedPtr<SCheckBox> ValueCheckBox;

	TSharedPtr<FString> SelectedString;
	uint8 Key;
	bool Value;

	void OnStateValueChanged(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo);
	void OnStateListOpened();
	void OnCheckStateChanged(ECheckBoxState CheckState);

	/** Property utilites */
	TSharedPtr<class IPropertyUtilities> PropertyUtilities;
};