#pragma once

#include "CoreMinimal.h"
#include "IAssetTools.h"
#include "IAssetTypeActions.h"
#include "Styling/SlateStyle.h"
#include "Modules/ModuleManager.h"

class FWindEditorModule : public IModuleInterface
{
public:
	FWindEditorModule() {}

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
