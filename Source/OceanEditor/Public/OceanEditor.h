#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IAssetTypeActions.h"
#include "IAssetTools.h"
#include "Styling/SlateStyle.h"

class FOceanEditorModule : public IModuleInterface
{
public:
	FOceanEditorModule() 
	{

	}

	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

private:
	EAssetTypeCategories::Type InfinityOceanCategory;
	TSharedPtr<FSlateStyleSet> OceanProfileStyleIcon;
	class FOceanProfileCreateAction* OceanAssetAction;
};
