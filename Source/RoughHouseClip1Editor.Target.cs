using UnrealBuildTool;
using System.Collections.Generic;

public class RoughHouseClip1EditorTarget : TargetRules
{
    public RoughHouseClip1EditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.Add("RoughHouseClip1");
    }
}
