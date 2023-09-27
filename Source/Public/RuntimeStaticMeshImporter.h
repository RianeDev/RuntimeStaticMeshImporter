// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

//#include "CoreMinimal.h"
#include "Engine.h"
#include "Modules/ModuleManager.h"

class RUNTIMESTATICMESHIMPORTER_API FRuntimeStaticMeshImporterModule : public IModuleInterface
{

public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};
