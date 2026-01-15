using UnrealBuildTool;

public class AutoPrefix : ModuleRules
{
    public AutoPrefix(ReadOnlyTargetRules Target) : base(Target)
    {
        // ❌ Type, bIsEditorOnly 줄 모두 제거
        //PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;   // 있어도 되고 없어도 됨
        DefaultBuildSettings = BuildSettingsVersion.V5;               // C++20 + 새 경고 레벨
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;   // 헤더 순서 규칙

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "UnrealEd",        //  ← GEditor, UImportSubsystem
            "EditorSubsystem", //  ← UImportSubsystem의 부모
            "AssetTools",
            "AssetRegistry",
            "LevelSequence"
        });

        // (선택) 인텔리센스용 include 경로
        PrivateIncludePathModuleNames.Add("UnrealEd");
    }
}