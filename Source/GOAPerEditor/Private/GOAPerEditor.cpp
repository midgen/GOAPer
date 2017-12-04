#include "GOAPerEditor.h"
#include "GOAPStateUI.h"
#include "Pins/GOAPAtomPin.h"
#include "Pins/GOAPAtomPinFactory.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "DetailCustomizations/GOAPStateCustomization.h"


IMPLEMENT_GAME_MODULE(FGOAPerEditorModule, GOAPerEditor);

DEFINE_LOG_CATEGORY(GOAPerEditor)

#define LOCTEXT_NAMESPACE "GOAPerEditor"

void FGOAPerEditorModule::StartupModule()
{
	UE_LOG(GOAPerEditor, Warning, TEXT("GOAPerEditorModule: Log Started"));

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	//Custom properties
	PropertyModule.RegisterCustomPropertyTypeLayout("GOAPAtom", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FGOAPStateCustomization::MakeInstance));

	//create your factory and shared pointer to it.
	TSharedPtr<FGOAPAtomPinFactory> GOAPAtomPinFactory = MakeShareable(new FGOAPAtomPinFactory());
	//and now register it.
	FEdGraphUtilities::RegisterVisualPinFactory(GOAPAtomPinFactory);
}

void FGOAPerEditorModule::ShutdownModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	//Custom properties
	PropertyModule.UnregisterCustomPropertyTypeLayout("GOAPAtom");
	UE_LOG(GOAPerEditor, Warning, TEXT("GOAPerEditorModule: Log Ended"));
}

#undef LOCTEXT_NAMESPACE