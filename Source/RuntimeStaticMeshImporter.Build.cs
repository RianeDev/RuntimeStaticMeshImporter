// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class RuntimeStaticMeshImporter : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
    }

    public RuntimeStaticMeshImporter(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
    new string[] {
                "RuntimeStaticMeshImporter/Public",
                Path.Combine(ThirdPartyPath, "assimp/include")
        // ... add public include paths required here ...
    }
);

        PrivateIncludePaths.AddRange(
			new string[] {
				"RuntimeStaticMeshImporter/Private",
				// ... add other private include paths required here ...
			}
		);


        PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "CoreUObject",
                "Engine",
                "RHI",
                "RenderCore",
                "ProceduralMeshComponent"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				"UMG",
                "StaticMeshDescription",
                "MeshDescription",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "Win64" : "Win32";
        PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, "assimp/lib", PlatformString, "assimp-vc140-mt.lib"));

		RuntimeDependencies.Add(Path.Combine(ThirdPartyPath, "assimp/bin", PlatformString, "assimp-vc140-mt.dll"));

        //PublicDelayLoadDLLs.Add("assimp-vc140-mt.dll");
        //PublicAdditionalLibraries.Add(Path.Combine(RuntimeStaticMeshImporter, "FreeImage.lib"));
    }
}
