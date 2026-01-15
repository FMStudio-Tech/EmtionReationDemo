#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FAutoPrefixModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** 실제 후처리 */
	void HandlePostImport(class UFactory* /*Factory*/, class UObject* CreatedObject);

	bool RenameIfNeeded(UObject* Asset, const TCHAR* Prefix);

	/** ImportSubsystem 델리게이트 해제용 */
	FDelegateHandle PostImportHandle;
};

static FString StripSuffix(const FString& Name, const TCHAR* Suffix);

static bool RenameWithPrefix(UObject* Asset, const TCHAR* Prefix, const TCHAR* OptionalSuffix);
