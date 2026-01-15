#include "AutoPrefixModule.h"

#include "Editor.h"
#include "Subsystems/ImportSubsystem.h"
#include "AssetToolsModule.h"
#include "ObjectTools.h"
#include "PhysicsEngine/PhysicsAsset.h"

#include "LevelSequence.h"
#include "Misc/PackageName.h"

void FAutoPrefixModule::StartupModule()
{
	if (!GEditor) return;

	UImportSubsystem* ImportSubsystem = GEditor->GetEditorSubsystem<UImportSubsystem>();
	PostImportHandle = ImportSubsystem->OnAssetPostImport.AddRaw(
		this, &FAutoPrefixModule::HandlePostImport);
}

void FAutoPrefixModule::ShutdownModule()
{
	if (GEditor)
	{
		if (UImportSubsystem* ImportSubsystem = GEditor->GetEditorSubsystem<UImportSubsystem>())
		{
			if (PostImportHandle.IsValid())
			{
				ImportSubsystem->OnAssetPostImport.Remove(PostImportHandle);
				PostImportHandle.Reset();
			}
		}
	}
}

void FAutoPrefixModule::HandlePostImport(UFactory*, UObject* Created)
{
    // 1) SkeletalMesh 임포트
    if (USkeletalMesh* SKM = Cast<USkeletalMesh>(Created))
    {
        // ─ 메시 이름
        RenameWithPrefix(SKM, TEXT("SK_"), nullptr);

        // ─ 연결된 Skeleton 이름
        RenameWithPrefix(SKM->GetSkeleton(),TEXT("SKEL_"), TEXT("_Skeleton"));

        // ─ 연결된 PhysicsAsset 이름
        RenameWithPrefix(SKM->GetPhysicsAsset(),TEXT("PHYS_"), TEXT("_PhysicsAsset"));

        //return;   // 여기서 끝내야 텍스처 등 다른 자산이 안 바뀜
        //RenameWithPrefix(SKM, TEXT("SK_"), nullptr);
        return;                 // 다른 에셋엔 손대지 않음
    }

    // 2) Skeleton 임포트
     if (USkeleton* Skeleton = Cast<USkeleton>(Created))
    {
        RenameWithPrefix(Skeleton, TEXT("SKEL_"), TEXT("_Skeleton"));
        return;
    }

    // 3) PhysicsAsset 임포트
     if (UPhysicsAsset* Phys = Cast<UPhysicsAsset>(Created))
    {
        // Phys 대신 Created(UObject*)를 넘겨 업-캐스트 문제 제거
        RenameWithPrefix(Created, TEXT("PHYS_"), TEXT("_PhysicsAsset"));
        return;
    }

    // 4) Texture 임포트
     if (UTexture* Tex = Cast<UTexture>(Created))
    {
        RenameWithPrefix(Tex, TEXT("T_"),nullptr);
        return;
    }
    // 5) AnimBlueprint 임포트
     if (UAnimBlueprint* ANBP = Cast<UAnimBlueprint>(Created))
    {
        RenameWithPrefix(ANBP, TEXT("ANBP_"), nullptr);
        return;
    }
    // 6) AnimSequence 임포트
     if (UAnimSequence* Seq = Cast<UAnimSequence>(Created))
    {
        RenameWithPrefix(Seq, TEXT("AS_"), nullptr);
        return;
    }
    // 7) LevelSequence 임포트
     if (ULevelSequence* LS = Cast<ULevelSequence>(Created))
    {
        RenameWithPrefix(LS, TEXT("LS_"), nullptr);
        return;
    }

    // 8) 그 밖의 모든 에셋(텍스처, 머티리얼 등)은 **향후 제작**
}
bool FAutoPrefixModule::RenameIfNeeded(UObject* Asset, const TCHAR* Prefix)
{
    if (!Asset || Asset->GetName().StartsWith(Prefix)) return false;

    const FString PkgPath = FPackageName::GetLongPackagePath(
        Asset->GetOutermost()->GetName());
    const FString NewName = FString(Prefix) + Asset->GetName();

    TArray<FAssetRenameData> Batch;
    Batch.Emplace(Asset, PkgPath, NewName);

    FAssetToolsModule& AssetTools =
        FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
    AssetTools.Get().RenameAssets(Batch);
    return true;
}

/* ① 뒤쪽 꼬리표를 잘라내는 헬퍼 */
static FString StripSuffix(const FString& Name, const TCHAR* Suffix)
{
    return (Suffix && Name.EndsWith(Suffix))
        ? Name.LeftChop(FCString::Strlen(Suffix))
        : Name;
}

/* ② 접두사를 붙이면서(중복 방지) 필요하면 꼬리표를 제거 */
static bool RenameWithPrefix(
    UObject* Asset,
    const TCHAR* Prefix,
    const TCHAR* OptionalSuffix = nullptr)      // ← 3번째 인수, 기본값 nullptr
{
    if (!Asset) return false;

    FString Base = StripSuffix(Asset->GetName(), OptionalSuffix);

    // 이미 접두사가 있으면 패스
    if (Base.StartsWith(Prefix)) return false;

    const FString NewName = FString(Prefix) + Base;
    const FString PkgPath = FPackageName::GetLongPackagePath(
        Asset->GetOutermost()->GetName());

    TArray<FAssetRenameData> Batch;
    Batch.Emplace(Asset, PkgPath, NewName);

    FAssetToolsModule& AT =
        FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
    return AT.Get().RenameAssets(Batch);
}

IMPLEMENT_MODULE(FAutoPrefixModule, AutoPrefix)