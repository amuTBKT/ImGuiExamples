#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

#include "ImGuiPluginTypes.h"
#include "implot.h"

class FImGuiEditorWidgetsModule : public IModuleInterface
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FImGuiEditorWidgetsModule, ImGuiEditorWidgets)

void FImGuiEditorWidgetsModule::StartupModule()
{
	SETUP_DEFAULT_IMGUI_ALLOCATOR();

	ImPlot::CreateContext();

	// flip Left and Right mouse button for panning+selection
	ImPlotInputMap& InputMap = ImPlot::GetInputMap();
	InputMap.Pan = ImGuiMouseButton_Right;
	InputMap.Select = ImGuiMouseButton_Left;
	InputMap.SelectCancel = ImGuiMouseButton_Right;
}

void FImGuiEditorWidgetsModule::ShutdownModule()
{
	ImPlot::DestroyContext();
}