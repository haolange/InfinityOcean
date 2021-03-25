#pragma once

#include "CoreMinimal.h"
#include "AssetTypeCategories.h"
#include "AssetTypeActions_Base.h"
#include "../../OceanRuntime/Public/OceanProfile.h"

class FAssetActions_CreateOceanProfile : public FAssetTypeActions_Base
{
public:
	FAssetActions_CreateOceanProfile(EAssetTypeCategories::Type InAssetCategory) { AssetCategory = InAssetCategory; }

	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_OceanProfile", "OceanProfile"); }
	virtual FColor GetTypeColor() const override { return FColor(65, 125, 255); }
	virtual UClass* GetSupportedClass() const override { return UOceanProfile::StaticClass(); }
	virtual uint32 GetCategories() override { return AssetCategory; /*return EAssetTypeCategories::Animation;*/ }

private:
	EAssetTypeCategories::Type AssetCategory;
};