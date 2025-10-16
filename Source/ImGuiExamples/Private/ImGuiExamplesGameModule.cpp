#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

#ifdef WITH_IMGUI
#include "ImGuiPluginTypes.h"
#endif

class FImGuiExamplesGameModule : public IModuleInterface
{
public:
	virtual bool IsGameModule() const override { return true; }

	virtual void StartupModule() override
	{
#ifdef WITH_IMGUI
		SETUP_DEFAULT_IMGUI_ALLOCATOR();
#endif
	}

	virtual void ShutdownModule() override
	{

	}
};

IMPLEMENT_PRIMARY_GAME_MODULE( FImGuiExamplesGameModule, ImGuiExamples, "ImGuiExamples" );
