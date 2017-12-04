
#pragma once
#include "SlateBasics.h"
#include "GOAPerSettings.h"
#include "SGraphPin.h"


class SGOAPAtomPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SGOAPAtomPin) {}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);
	//this override is used to display slate widget used forcustomization.
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	void OnStateValueChanged(TSharedPtr<FString> ItemSelected, ESelectInfo::Type SelectInfo);

	void SetValue(uint8 aKey);
private:
	TArray<TSharedPtr<FString>> AvailableOptions;

	uint8 Key;
	bool Value;

	UGOAPerSettings* GOAPSettings = GetMutableDefault<UGOAPerSettings>();
};