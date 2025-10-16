#ifdef WITH_IMGUI

#include "Misc/Paths.h"
#include "ImGuiSubsystem.h"

namespace ImGuiTestWidget
{
    static void Initialize()
    {
    }

    static void Tick(FImGuiTickContext* Context)
    {
		FImGuiTickScope Scope{ Context };

		if (ImGui::Begin("Test Widget"))
		{
            UImGuiSubsystem* ImGuiSubsystem = UImGuiSubsystem::Get();
            
            static ImFont* CustomFont = ImGuiSubsystem->GetSharedFontAtlas()->AddFontFromFileTTF(
                TCHAR_TO_ANSI(*(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"))), 20.f);
            if (CustomFont)
            {
                ImGui::PushFont(CustomFont, 20.f);
                ImGui::Text("Text using custom font");
                ImGui::PopFont();
            }
            ImGui::Text("Text using default font");

            ImGui::NewLine();

            auto UnrealLogo = ImGuiSubsystem->RegisterOneFrameResource(IMGUI_STYLE_ICON("CoreStyle", "UnrealCircle.Thick"), 32.f * ImGui::GetStyle().FontScaleMain);
            ImGui::Image(UnrealLogo.Id, UnrealLogo.Size, UnrealLogo.UV0, UnrealLogo.UV1);
            ImGui::SameLine(); ImGui::SetCursorPosY(ImGui::GetCursorPosY() + UnrealLogo.Size.y * 0.25f);
            ImGui::Text("Slate icon");

            ImGui::NewLine();

            static int32 ClickCount = 0;
            if (ImGui::Button("Click"))
            {
                ++ClickCount;
            }
            ImGui::SameLine();
            ImGui::Text("Count: %i", ClickCount);
		}
		ImGui::End();
    }

    FStaticWidgetRegisterParams Params =
    {
        .InitFunction = &Initialize,
        .TickFunction = &Tick,
        .WidgetName = "Test",
        .WidgetDescription = "Test Widget"
    };
    IMGUI_REGISTER_MAIN_WINDOW_WIDGET(Params);
}

#endif //#if WITH_IMGUI
