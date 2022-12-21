#if WITH_IMGUI

#include "StaticWidgets.h"

namespace HelloStaticWidget
{
    static void Initialize()
    {
    }

    static void RegisterOneFrameResources()
    {
		FImGuiRuntimeModule& ImGuiRuntimeModule = FModuleManager::GetModuleChecked<FImGuiRuntimeModule>("ImGuiRuntime");
    }

    static void Tick(ImGuiContext* Context)
    {
		FImGuiTickScope Scope{ Context };

		if (ImGui::Begin("Hello Widget", nullptr, ImGuiWindowFlags_None))
		{
            static int32 ClickCount = 0;
            if (ImGui::Button("Click"))
            {
                ++ClickCount;
            }
            ImGui::Text("ClickCount: %i", ClickCount);
		}
		ImGui::End();
    }

    IMGUI_REGISTER_STATIC_WIDGET(Initialize, Tick);
}

#endif //#if WITH_IMGUI
