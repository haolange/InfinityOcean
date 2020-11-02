#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Factories/Factory.h"

#include "OceanProfileFactory.generated.h"

UCLASS()
class OCEANEDITOR_API UOceanProfileFactory : public UFactory
{
	GENERATED_BODY()
	
	UOceanProfileFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
