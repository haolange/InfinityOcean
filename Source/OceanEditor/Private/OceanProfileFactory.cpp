#include "OceanProfileFactory.h"
#include "AssetTypeCategories.h"
#include "OceanRuntime\Public\OceanProfile.h"

UOceanProfileFactory::UOceanProfileFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UOceanProfile::StaticClass();
}

UObject* UOceanProfileFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UOceanProfile>(InParent, Class, Name, Flags);
}