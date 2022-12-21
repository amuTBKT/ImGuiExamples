#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "ImGuiPluginTypes.h"

class FImGuiExamplesGameModule : public IModuleInterface
{
public:
	virtual bool IsGameModule() const override { return true; }

	virtual void StartupModule() override
	{
		SETUP_DEFAULT_IMGUI_ALLOCATOR();
	}
	virtual void ShutdownModule() override
	{

	}
};

IMPLEMENT_PRIMARY_GAME_MODULE( FImGuiExamplesGameModule, ImGuiExamples, "ImGuiExamples" );
