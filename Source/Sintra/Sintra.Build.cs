// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class Sintra : ModuleRules
{
    /*
	public Sintra(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}*/

    public string ModulePath
    {
        get { return ModuleDirectory; }
    }

    public string ThirdPartyPath
    {
        get
        {
            return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/"));
        }
    }

    public Sintra(ReadOnlyTargetRules Target) : base(Target)
    {

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Landscape" });

        PrivateDependencyModuleNames.AddRange(new string[] { });
        LoadGDAL(Target);
        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
        bEnableUndefinedIdentifierWarnings = false;

    }

    public bool LoadGDAL(ReadOnlyTargetRules Target)
    {

        bool isLibrarySupported = false;

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            isLibrarySupported = true;

            //string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
            string LibrariesPath = Path.Combine(ThirdPartyPath, "gdal-2.3.1");

            //PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "port", "cpl.lib"));
            //PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "ogr", "ogr.lib"));
            //PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "ogr", "ogrsf_frmts", "ogrsf_frmts.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "gdal_i.lib"));
        }

        if (isLibrarySupported)
        {
            // Include path
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "gdal-2.3.1"));
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "gdal-2.3.1", "ogr"));
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "gdal-2.3.1", "apps"));
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "gdal-2.3.1", "port"));
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "gdal-2.3.1", "gcore"));
            PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "gdal-2.3.1", "ogr", "ogrsf_frmts"));
        }

        PublicDefinitions.Add(string.Format("WITH_GDAL_BINDING={0}", isLibrarySupported ? 1 : 0));

        return isLibrarySupported;



    }
}
